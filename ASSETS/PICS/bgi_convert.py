from PIL import Image
import struct
import sys

# Палитра BGI (16 цветов) в RGB
BGI_PALETTE = [
    (0, 0, 0),       # 0 Black
    (0, 0, 170),     # 1 Blue
    (0, 170, 0),     # 2 Green
    (0, 170, 170),   # 3 Cyan
    (170, 0, 0),     # 4 Red
    (170, 0, 170),   # 5 Magenta
    (170, 85, 0),    # 6 Brown
    (170, 170, 170), # 7 Light Gray
    (85, 85, 85),    # 8 Dark Gray
    (85, 85, 255),   # 9 Light Blue
    (85, 255, 85),   # 10 Light Green
    (85, 255, 255),  # 11 Light Cyan
    (255, 85, 85),   # 12 Light Red
    (255, 85, 255),  # 13 Light Magenta
    (255, 255, 85),  # 14 Yellow
    (255, 255, 255)  # 15 White
]

def closest_color(rgb):
    r, g, b = rgb
    min_dist = float('inf')
    best_idx = 0
    for idx, (pr, pg, pb) in enumerate(BGI_PALETTE):
        dr = r - pr
        dg = g - pg
        db = b - pb
        dist = dr*dr + dg*dg + db*db
        if dist < min_dist:
            min_dist = dist
            best_idx = idx
    return best_idx

def write_4bit_bmp(image_path, output_path):
    img = Image.open(image_path).convert('RGB')
    w, h = img.size

    if w % 2 != 0:
        print("Error: image width must be even for 4-bit BMP without padding.")
        return

    # Формируем палитру BMP: 16 записей по 4 байта (B, G, R, 0)
    palette_bytes = b''
    for r, g, b in BGI_PALETTE:
        palette_bytes += bytes([b, g, r, 0])

    # Размер строки пикселей в байтах, выровненный до 4
    row_size = ((w // 2) + 3) & ~3
    pixel_data = bytearray()

    # BMP хранит строки снизу вверх
    for y in range(h - 1, -1, -1):
        row = bytearray()
        for x in range(0, w, 2):
            idx_left = closest_color(img.getpixel((x, y)))
            idx_right = closest_color(img.getpixel((x + 1, y)))
            row.append((idx_left << 4) | idx_right)
        # Дополняем строку до row_size нулями
        while len(row) < row_size:
            row.append(0)
        pixel_data.extend(row)

    # Заголовок BMP (14 байт)
    bfType = b'BM'
    bfSize = 14 + 40 + len(palette_bytes) + len(pixel_data)
    bfOffBits = 14 + 40 + len(palette_bytes)
    file_header = struct.pack('<2sIHHI', bfType, bfSize, 0, 0, bfOffBits)

    # Информационный заголовок (40 байт)
    info_header = struct.pack('<IiiHHIIiiII',
                              40,             # biSize
                              w,              # biWidth
                              h,              # biHeight
                              1,              # biPlanes
                              4,              # biBitCount
                              0,              # biCompression (BI_RGB)
                              len(pixel_data),# biSizeImage
                              0,              # biXPelsPerMeter
                              0,              # biYPelsPerMeter
                              16,             # biClrUsed
                              0)              # biClrImportant

    with open(output_path, 'wb') as f:
        f.write(file_header)
        f.write(info_header)
        f.write(palette_bytes)
        f.write(pixel_data)

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage: python bgi_convert.py <input_image> <output.bmp>")
        sys.exit(1)
    write_4bit_bmp(sys.argv[1], sys.argv[2])
    print(f"Saved {sys.argv[2]} as 4-bit BMP with BGI palette")