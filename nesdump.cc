
#include "nes.hh"

#include <iostream>
#include <variant>

struct ItemError {};
struct ItemInfo {};
struct ItemPrg { int bank; };
struct ItemChr { int bank; };

using Item = std::variant<ItemError, ItemInfo, ItemPrg, ItemChr>;

Item parse_item(std::string_view item_str)
{
    std::size_t const colon_pos = item_str.find_first_of(':');

    std::string_view const item_name = (colon_pos == std::string_view::npos)
        ? item_str
        : item_str.substr(0, colon_pos);

    std::string_view const item_part = (colon_pos == std::string_view::npos)
        ? ""
        : item_str.substr(colon_pos+1);

    if (item_name.compare("info") == 0)
    {
        Item result = ItemInfo {};
        return result;
    }

    if (item_name.compare("prg") == 0)
    {
        int bank = -1;

        if (!item_part.empty())
            bank = std::stoi(std::string { item_part }, nullptr, 0);

        Item result = ItemPrg { bank };
        return result;
    }

    if (item_name.compare("chr") == 0)
    {
        int bank = -1;

        if (!item_part.empty())
            bank = std::stoi(std::string { item_part }, nullptr, 0);

        Item result = ItemChr { bank };
        return result;
    }

    Item result = ItemError {};
    return result;
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <rom.nes> <item>" << std::endl;
        return 1;
    }

    try
    {
        NesRom rom = NesRom::from_ines_file(argv[1]);
        Item item = parse_item(argv[2]);

        MapperInfo const* const mapper_info = get_mapper_info(rom.get_mapper_id());

        std::visit([&] (auto& item)
        {
            using T = std::decay_t<decltype(item)>;

            if constexpr (std::is_same_v<T, ItemInfo>)
            {
                std::cout << "ROM Info:" << std::endl;
                std::cout << "  Mapper:       " << mapper_info->name << " (" << std::dec << (unsigned) rom.get_mapper_id() << ")" << std::endl;
                std::cout << "  iNES flags:   " << std::hex << (unsigned) rom.get_ines_flags() << std::endl;
                std::cout << "  PRG-ROM size: " << std::dec << rom.get_prg_rom().size()/1024 << "K (" << std::dec << mapper_info->count_prg_banks(rom) << " bank(s))" << std::endl;
                std::cout << "  CHR-ROM size: " << std::dec << rom.get_chr_rom().size()/1024 << "K (" << std::dec << mapper_info->count_chr_banks(rom) << " bank(s))" << std::endl;
            }

            if constexpr (std::is_same_v<T, ItemPrg>)
            {
                int bank = item.bank;

                if (bank < 0)
                {
                    // Write all
                    std::cout.write(reinterpret_cast<char const*>(rom.get_prg_rom().data()), rom.get_prg_rom().size());
                }
                else
                {
                    if (mapper_info->prg_bank_size == 0)
                        throw std::runtime_error("This mapper isn't known to feature PRG-ROM banking");

                    if ((unsigned) bank >= mapper_info->count_prg_banks(rom))
                        throw std::runtime_error("Bank number is too high");

                    std::cout.write(reinterpret_cast<char const*>(mapper_info->get_prg_bank(rom, bank)), mapper_info->prg_bank_size);
                }
            }

            if constexpr (std::is_same_v<T, ItemChr>)
            {
                int bank = item.bank;

                if (bank < 0)
                {
                    // Write all
                    std::cout.write(reinterpret_cast<char const*>(rom.get_chr_rom().data()), rom.get_chr_rom().size());
                }
                else
                {
                    if (mapper_info->chr_bank_size == 0)
                        throw std::runtime_error("This mapper isn't known to feature CHR-ROM banking");

                    if ((unsigned) bank >= mapper_info->count_chr_banks(rom))
                        throw std::runtime_error("Bank number is too high");

                    std::cout.write(reinterpret_cast<char const*>(mapper_info->get_chr_bank(rom, bank)), mapper_info->chr_bank_size);
                }
            }
        }, item);
    }
    catch (std::runtime_error const& e)
    {
        std::cerr << "An error occured:" << std::endl;
        std::cerr << "  " << e.what() << std::endl;

        return 2;
    }
}
