echo "Getting base for the menu"
make
TEXT_BASE=$(python pnach_creator.py -input menu.elf)
TEXT_BASE=${TEXT_BASE} make
python pnach_creator.py -input menu_${TEXT_BASE}.elf -base ${TEXT_BASE}
