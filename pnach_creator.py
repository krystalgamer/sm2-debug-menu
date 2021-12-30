from elftools.elf.elffile import ELFFile
from elftools.elf.sections import SymbolTableSection
import argparse

from enum import Enum


class PatchType(Enum):
    NORMAL = 0
    JAL = 1

GAME_BASE = 0x00100000


patches = {
        'pause_menu_key_handler': (0x00657244, PatchType.NORMAL, 'Vtable entry'),
        'debug_menu_render': (0x00519234, PatchType.JAL, 'Unused nullsub before perf info'),
        'game_unpauser_unpause_hook': (0x00353C60, PatchType.JAL, 'Calling game_pause after unpause makes the pause front_end appear so hook the unpause call'),
        'sub_3E9A90': (0x00395DA8, PatchType.JAL, 'Nops the call to sub_3E9A90 when debug menu is enabled because it handles START and SELECT for some reason')
        }

def base_parser(i):
    return int(i, base=16)

def path_parser(i):
    return open(i, 'rb')


def calculate_base(ba):

    l = len(ba)
    l += len(ba)%4

    if l > GAME_BASE:
        raise ValueError('Too big text section')
    return GAME_BASE-l

def get_base(fp):
    elf = ELFFile(fp)

    text_section = elf.get_section_by_name('.text')
    data = text_section.data()
    return calculate_base(data)


def get_symbol_tables(elf):
    symbol_tables = [(idx, s) for idx, s in enumerate(elf.iter_sections())
                                     if isinstance(s, SymbolTableSection)]

    return symbol_tables
    lst = lst[:]
    res = dict()

def get_symbols(symbol_tables, lst):

    lst = lst[:]
    res = dict()
    for section_index, section in symbol_tables:
        for nsym, symbol in enumerate(section.iter_symbols()):

            if symbol['st_info']['type'] != 'STT_FUNC':
                continue


            if symbol.name in lst:
                res[symbol.name] = symbol['st_value']
                lst.remove(symbol.name)
    for left in lst:
        print(f'{left} was not found')

    return res

def get_fdata(symbol_tables):
    for section_index, section in symbol_tables:
        for nsym, symbol in enumerate(section.iter_symbols()):

            if symbol.name == '_fdata':
                return symbol['st_value']


def create_patch(fp, base):
    elf = ELFFile(fp)
    symbol_tables = get_symbol_tables(elf)
    fdata = get_fdata(symbol_tables)

    text_section = elf.get_section_by_name('.text')
    data = text_section.data()
    data = data[:fdata-GAME_BASE]

    data = data + bytes([0]*(len(data)%4))
    data = [data[x:x+4] for x in range(0, len(data), 4)]
    data = map(lambda x: int.from_bytes(x, 'little'), data)

    lines = []

    symbols = get_symbols(symbol_tables, list(patches.keys()))
    for i, entry in enumerate(data):
        line = f'patch=1,EE,{i*4+base:08X},word,{entry:08X}'
        lines.append(line)

    for symbol, value in symbols.items():

        patch_value = value
        if patches[symbol][1] == PatchType.JAL:
            if patch_value & 0xFF000000:
                raise ValueError(f'{symbol} is not reachable with JAL patch')
            patch_value = 0x0C000000 | ((patch_value >> 2) & 0x00FFFFFF)

        address = patches[symbol][0]
        line = f'patch=1,EE,{address:08X},word,{patch_value:08X}'
        lines.append(line)


    ## manual patches
    lines.append('patch=1,EE,00353E60,word,00000000')#patch a check in handle_pause_stuff that when the UI is not rendered swallows input
    return '\n'.join(lines)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-input', type=path_parser, required=True)
    parser.add_argument('-base', type=base_parser)
    args = parser.parse_args()


    if args.base is None:
        b = get_base(args.input)
        print(f'{b:x}')
    else:
        p = create_patch(args.input, args.base)

        print('Patch done, writting...', end='')
        with open('/mnt/d/emulators/pcsx2-v1.7.0-dev-60-g802318399-windows-x86/cheats/E2DFCE12.PNACH', 'w') as fp:
            fp.write(p)
        print('Done')




if __name__ == '__main__':
    main()
