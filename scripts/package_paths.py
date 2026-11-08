"""Portable package paths and lexical symlink confinement."""
from pathlib import PurePosixPath


def relative_path(value):
    path = PurePosixPath(value)
    if (not value or value == '.' or path.is_absolute() or '..' in path.parts
            or '\\' in value or ':' in value or '\0' in value or str(path) != value):
        raise ValueError(f'Invalid package path: {value}')
    return value


def validate_inventory_links(inventory):
    paths = set()
    links = {}
    for item in inventory:
        name = relative_path(item['path'])
        if name in paths:
            raise ValueError(f'Duplicate inventory entry: {name}')
        paths.add(name)
        if 'symlink' in item:
            target = item['symlink']
            if (not isinstance(target, str) or not target or '\\' in target
                    or ':' in target or '\0' in target
                    or PurePosixPath(target).is_absolute()):
                raise ValueError(f'Invalid symlink target: {name}')
            links[name] = PurePosixPath(target).parts

    def resolve(parts, active):
        resolved = []
        for part in parts:
            if part == '..':
                if not resolved:
                    raise ValueError('Symlink escapes package root')
                resolved.pop()
            elif part != '.':
                name = '/'.join([*resolved, part])
                if name in links:
                    if name in active or len(active) >= 64:
                        raise ValueError(f'Symlink cycle or excessive chain: {name}')
                    resolved = resolve([*resolved, *links[name]], active | {name})
                else:
                    resolved.append(part)
        return resolved

    # Inventories list the physical tree, not duplicate entries through aliases.
    # Reject writes through a symlink parent before creating any output files.
    for name in paths:
        if any(str(parent) in links for parent in PurePosixPath(name).parents):
            raise ValueError(f'Inventory entry traverses a symlink parent: {name}')

    for name in paths:
        resolve(PurePosixPath(name).parts, set())
