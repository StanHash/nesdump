
#include "nes.hh"

#include <fstream>
#include <array>

static constexpr std::size_t const KIBI = 1024;

NesRom NesRom::from_ines_file(char const* filename)
{
    static constexpr std::size_t INES_PRG_BANK_SIZE = KIBI*16;
    static constexpr std::size_t INES_CHR_BANK_SIZE = KIBI*8;

    NesRom result {};

    std::ifstream f(filename, std::ios::in | std::ios::binary);

    if (!f.is_open())
        throw std::runtime_error("Couldn't open file for binary read");

    std::array<byte_type, 16> ines_header {};

    if (!f.read(reinterpret_cast<char*>(ines_header.data()), ines_header.size()))
        throw std::runtime_error("Couldn't read iNES header");

    if (ines_header[0] != 'N' || ines_header[1] != 'E' || ines_header[2] != 'S' || ines_header[3] != '\x1A')
        throw std::runtime_error("file doesn't contain valid iNES header (bad magic)");

    std::size_t const prg_banks = ines_header[4];
    result.m_prg.resize(prg_banks*INES_PRG_BANK_SIZE);

    if (!f.read(reinterpret_cast<char*>(result.m_prg.data()), result.m_prg.size()))
        throw std::runtime_error("Couldn't read PRG-ROM (reached EOF early)");

    std::size_t const chr_banks = ines_header[5];
    result.m_chr.resize(chr_banks*INES_CHR_BANK_SIZE);

    if (!f.read(reinterpret_cast<char*>(result.m_chr.data()), result.m_chr.size()))
        throw std::runtime_error("Couldn't read CHR-ROM (reached EOF early)");

    result.m_mapper_id = ((ines_header[6] & 0xF0) >> 4) | (ines_header[7] & 0xF0);
    result.m_ines_flags = (ines_header[6] & 0x0F) | ((ines_header[7] & 0x0F) << 4);

    return result;
}

static MapperInfo const s_mapper_bad { "Unsupported", 0, 0 };
static MapperInfo const s_mapper_nrom { "NROM", 0, 0 };
static MapperInfo const s_mapper_mmc1 { "MMC1", 16*KIBI, 4*KIBI };
static MapperInfo const s_mapper_unrom { "UNROM", 16*KIBI, 0 };
static MapperInfo const s_mapper_cnrom { "CNROM", 0, 8*KIBI };
static MapperInfo const s_mapper_mmc3 { "MMC3", 8*KIBI, 1*KIBI };
static MapperInfo const s_mapper_mmc5 { "MMC5", 8*KIBI, 1*KIBI };
static MapperInfo const s_mapper_aorom { "AOROM", 32*KIBI, 0 };
static MapperInfo const s_mapper_mmc2 { "MMC2", 8*KIBI, 4*KIBI };
static MapperInfo const s_mapper_mmc4 { "MMC4", 16*KIBI, 4*KIBI };
static MapperInfo const s_mapper_bandai { "BANDAI", 16*KIBI, 1*KIBI };

MapperInfo const* get_mapper_info(std::uint8_t mapper_id)
{
    switch (mapper_id)
    {

    case 0: return &s_mapper_nrom;
    case 1: return &s_mapper_mmc1;
    case 2: return &s_mapper_unrom;
    case 3: return &s_mapper_cnrom;
    case 4: return &s_mapper_mmc3;
    case 5: return &s_mapper_mmc5;
    case 7: return &s_mapper_aorom;
    case 9: return &s_mapper_mmc2;
    case 10: return &s_mapper_mmc4;
    case 16: return &s_mapper_bandai;

    default:
        return &s_mapper_bad;

    }
}

