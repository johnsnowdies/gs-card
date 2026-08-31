#!/usr/bin/env python3
"""Generate OBJECTS.SOL — random gas clouds, black holes, nebulae
   placed between stars from SYSTEM.SOL."""

import random
import sys
import os

sys.path.insert(0, os.path.dirname(__file__))

OBJ_TYPES = [
    ("GASCLOUD",  0, 30, 80),
    ("BLACKHOLE", 1, 15, 40),
    ("NEBULA",    2, 40, 90),
]

def load_systems(path):
    systems = []
    with open(path) as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            parts = line.split(';')
            if len(parts) >= 3:
                systems.append((int(parts[0]), int(parts[1]), int(parts[2])))
    return systems

def generate_objects(systems, count=15, min_dist=80):
    """Generate `count` objects placed between stars, at least `min_dist` from any star."""
    if not systems:
        return []

    xs = [s[0] for s in systems]
    ys = [s[1] for s in systems]
    zs = [s[2] for s in systems]

    x_min, x_max = min(xs), max(xs)
    y_min, y_max = min(ys), max(ys)
    z_min, z_max = min(zs), max(zs)

    # Expand bounds slightly so objects can be "between" stars
    margin = 50
    x_min -= margin; x_max += margin
    y_min -= margin; y_max += margin
    z_min -= margin; z_max += margin

    objects = []
    attempts = 0
    max_attempts = count * 50

    while len(objects) < count and attempts < max_attempts:
        attempts += 1
        ox = random.randint(x_min, x_max)
        oy = random.randint(y_min, y_max)
        oz = random.randint(z_min, z_max)

        # Check distance to nearest star
        too_close = False
        for sx, sy, sz in systems:
            dx = ox - sx
            dy = oy - sy
            dz = oz - sz
            dist = (dx*dx + dy*dy + dz*dz) ** 0.5
            if dist < min_dist:
                too_close = True
                break

        if too_close:
            continue

        # Pick a random type
        name, type_id, r_min, r_max = random.choice(OBJ_TYPES)
        radius = random.randint(r_min, r_max)

        objects.append((ox, oy, oz, radius, type_id))

    return objects

def main():
    systems_path = os.path.join(os.path.dirname(__file__), 'ASSETS', 'SYSTEM.SOL')
    output_path = os.path.join(os.path.dirname(__file__), 'ASSETS', 'OBJECTS.SOL')

    if not os.path.exists(systems_path):
        print(f"ERROR: {systems_path} not found")
        sys.exit(1)

    systems = load_systems(systems_path)
    print(f"Loaded {len(systems)} systems from {systems_path}")

    objects = generate_objects(systems, count=15, min_dist=80)
    print(f"Generated {len(objects)} objects")

    with open(output_path, 'w') as f:
        for ox, oy, oz, r, t in objects:
            f.write(f"{ox};{oy};{oz};{r};{t}\n")

    print(f"Written to {output_path}")

    # Print summary
    for ox, oy, oz, r, t in objects:
        type_name = ["GASCLOUD", "BLACKHOLE", "NEBULA"][t]
        print(f"  {type_name:10s} @ ({ox:4d}, {oy:4d}, {oz:4d})  r={r}")

if __name__ == '__main__':
    main()