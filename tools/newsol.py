import struct
import math

def read_systems(filename):
    systems = []
    with open(filename, 'r') as f:
        for line in f:
            parts = line.strip().split(';')
            if len(parts) < 3:
                continue
            x, y, z = map(int, parts[0:3])
            faction = int(parts[3]) if len(parts) > 3 else 0
            is_shipyard = int(parts[4]) if len(parts) > 4 else 0
            is_gas_station = int(parts[5]) if len(parts) > 5 else 0
            sector = int(parts[6]) if len(parts) > 6 else 0
            systems.append({
                'x': x, 'y': y, 'z': z,
                'faction': faction,
                'is_shipyard': is_shipyard,
                'is_gas_station': is_gas_station,
                'sector': sector
            })
    return systems

def dist_sq(a, b):
    dx = a['x'] - b['x']
    dy = a['y'] - b['y']
    dz = a['z'] - b['z']
    return dx*dx + dy*dy + dz*dz

def build_edges(systems, max_dist=130.0, max_neighbors=15):
    """Возвращает список рёбер в виде (idx1, idx2)"""
    n = len(systems)
    edges = []
    # Для каждой системы ищем ближайших соседей в радиусе, но не более max_neighbors
    # В Turbo C порядок перебора: для каждой i перебираем все j, берём первые max_neighbors в радиусе.
    for i in range(n):
        neighbors = []
        for j in range(n):
            if i == j:
                continue
            d = dist_sq(systems[i], systems[j])
            if d <= max_dist * max_dist:
                neighbors.append((d, j))
        # сортируем по расстоянию, берём не более max_neighbors
        neighbors.sort()
        neighbors = neighbors[:max_neighbors]
        for d, j in neighbors:
            edges.append((i, j))
    # Убираем дубликаты (если ребро (i,j) и (j,i) оба попали)
    # В Turbo C алгоритм строит ориентированные дуги: для каждой i свои соседи.
    # Поэтому сохраним все ориентированные рёбра. Для отсечения пересечений будем считать
    # неориентированное ребро {min,max}, чтобы не проверять дважды.
    return edges

def segments_intersect_3d(a, b, c, d):
    """Возвращает True, если отрезки AB и CD пересекаются."""
    x1, y1, z1 = a['x'], a['y'], a['z']
    x2, y2, z2 = b['x'], b['y'], b['z']
    x3, y3, z3 = c['x'], c['y'], c['z']
    x4, y4, z4 = d['x'], d['y'], d['z']

    dx1 = x2 - x1
    dy1 = y2 - y1
    dz1 = z2 - z1
    dx2 = x4 - x3
    dy2 = y4 - y3
    dz2 = z4 - z3
    dx3 = x3 - x1
    dy3 = y3 - y1
    dz3 = z3 - z1

    # cross product of directions
    cx = dy1 * dz2 - dz1 * dy2
    cy = dz1 * dx2 - dx1 * dz2
    cz = dx1 * dy2 - dy1 * dx2
    norm = cx*cx + cy*cy + cz*cz
    if norm < 1e-12:
        return False  # parallel

    # coplanarity
    mixed = dx1 * (dy2 * dz3 - dz2 * dy3) \
          - dy1 * (dx2 * dz3 - dz2 * dx3) \
          + dz1 * (dx2 * dy3 - dy2 * dx3)
    if abs(mixed) > 1e-9:
        return False  # skew

    # solve using XY, XZ, or YZ projection
    det = dx1 * dy2 - dy1 * dx2
    if abs(det) > 1e-12:
        t = ((dx3 * dy2) - (dy3 * dx2)) / det
        s = ((dx3 * dy1) - (dy3 * dx1)) / det
        if 0.0 <= t <= 1.0 and 0.0 <= s <= 1.0:
            return True
    else:
        det_xz = dx1 * dz2 - dz1 * dx2
        if abs(det_xz) > 1e-12:
            t = ((dx3 * dz2) - (dz3 * dx2)) / det_xz
            s = ((dx3 * dz1) - (dz3 * dx1)) / det_xz
            if 0.0 <= t <= 1.0 and 0.0 <= s <= 1.0:
                return True
        else:
            det_yz = dy1 * dz2 - dz1 * dy2
            if abs(det_yz) > 1e-12:
                t = ((dy3 * dz2) - (dz3 * dy2)) / det_yz
                s = ((dy3 * dz1) - (dz3 * dy1)) / det_yz
                if 0.0 <= t <= 1.0 and 0.0 <= s <= 1.0:
                    return True
    return False

def build_conflict_graph(systems, edges):
    """Строит граф конфликтов: для каждого ребра список индексов пересекающихся рёбер."""
    m = len(edges)
    conflict = [set() for _ in range(m)]
    # Перебираем все пары рёбер (без повторений)
    for i in range(m):
        a_idx, b_idx = edges[i]
        A = systems[a_idx]
        B = systems[b_idx]
        for j in range(i+1, m):
            c_idx, d_idx = edges[j]
            # пропускаем, если рёбра имеют общую вершину (они не могут пересекаться в контексте нашей задачи)
            if a_idx == c_idx or a_idx == d_idx or b_idx == c_idx or b_idx == d_idx:
                continue
            C = systems[c_idx]
            D = systems[d_idx]
            if segments_intersect_3d(A, B, C, D):
                conflict[i].add(j)
                conflict[j].add(i)
    return conflict

def iterative_remove_conflicts(conflict):
    """Вариант 2: итеративно удаляем ребро с максимальной степенью конфликтов, обновляя соседей."""
    m = len(conflict)
    degrees = [len(s) for s in conflict]
    alive = [True] * m
    # Очередь с приоритетом по степени (можно просто каждый раз искать максимум)
    while True:
        # найдём живое ребро с максимальной степенью
        max_deg = -1
        max_idx = -1
        for i in range(m):
            if alive[i] and degrees[i] > max_deg:
                max_deg = degrees[i]
                max_idx = i
        if max_deg == 0:
            break  # конфликтов не осталось
        # удаляем это ребро
        alive[max_idx] = False
        # уменьшаем степени соседей
        for other in conflict[max_idx]:
            if alive[other]:
                degrees[other] -= 1
        degrees[max_idx] = 0
    return [i for i in range(m) if alive[i]]

def write_bin(systems, edges, kept_indices, out_filename):
    n = len(systems)
    # Сначала сгруппируем рёбра по исходной системе (ориентированные)
    # edges может быть ориентированным списком (i->j). В Python мы строили ориентированные,
    # но для отсечения использовали все. Теперь нужно оставить только те, которые выжили.
    # kept_indices относятся к полному списку edges (включая оба направления? Мы строили ориентированные,
    # поэтому для каждой системы будут свои исходящие рёбра). После отсечения мы должны сохранить
    # для каждой системы список target и cost=0.
    # Для простоты создадим словарь: system_idx -> list of (target_idx)
    from collections import defaultdict
    adjacency = defaultdict(list)
    for idx in kept_indices:
        src, dst = edges[idx]
        adjacency[src].append(dst)
    # Ограничим количество соседей (не более 15, как в исходном коде), хотя после отсечения их может быть <=15.
    with open(out_filename, 'wb') as f:
        # заголовок
        f.write(struct.pack('<H', n))
        for i, sys in enumerate(systems):
            # координаты и флаги
            f.write(struct.pack('<hhhHHHH', 
                sys['x'], sys['y'], sys['z'],
                sys['faction'], sys['is_shipyard'], sys['is_gas_station'], sys['sector']))
            targets = adjacency.get(i, [])
            # ограничим на всякий случай
            targets = targets[:15]
            f.write(struct.pack('<B', len(targets)))
            for t in targets:
                f.write(struct.pack('<Hh', t, 0))
    print(f"Written {out_filename}: {n} systems, {len(kept_indices)} edges kept.")

if __name__ == '__main__':
    systems = read_systems('SYSTEM.SOL')
    print(f"Systems: {len(systems)}")
    edges = build_edges(systems)  # ориентированные
    print(f"Original edges: {len(edges)}")
    conflict = build_conflict_graph(systems, edges)
    kept = iterative_remove_conflicts(conflict)
    print(f"Kept edges after removal: {len(kept)}")
    write_bin(systems, edges, kept, 'SYSTEM.BIN')