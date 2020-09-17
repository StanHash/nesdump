
#pragma once

#include <cstdint>
#include <vector>
#include <string>

using byte_type = std::uint8_t;

struct NesRom
{
    std::uint8_t get_mapper_id() const
    {
        return m_mapper_id;
    }

    std::uint8_t get_ines_flags() const
    {
        return m_ines_flags;
    }

    std::vector<byte_type> const& get_prg_rom() const
    {
        return m_prg;
    }

    std::vector<byte_type> const& get_chr_rom() const
    {
        return m_chr;
    }

    static NesRom from_ines_file(std::string const& filename)
    {
        return from_ines_file(filename.c_str());
    }

    static NesRom from_ines_file(char const* filename);

private:
    NesRom() = default;

private:
    std::vector<byte_type> m_prg {};
    std::vector<byte_type> m_chr {};

    std::uint8_t m_mapper_id = 0;
    std::uint8_t m_ines_flags = 0;
};

struct MapperInfo
{
    char const* name;
    std::size_t prg_bank_size;
    std::size_t chr_bank_size;

    unsigned count_prg_banks(NesRom const& rom) const
    {
        if (prg_bank_size == 0)
            return 1;

        return rom.get_prg_rom().size() / prg_bank_size;
    }

    unsigned count_chr_banks(NesRom const& rom) const
    {
        if (chr_bank_size == 0)
            return 1;

        return rom.get_chr_rom().size() / chr_bank_size;
    }

    byte_type const* get_prg_bank(NesRom const& rom, unsigned bank) const
    {
        return rom.get_prg_rom().data() + bank*prg_bank_size;
    }

    byte_type const* get_chr_bank(NesRom const& rom, unsigned bank) const
    {
        return rom.get_chr_rom().data() + bank*chr_bank_size;
    }
};

MapperInfo const* get_mapper_info(std::uint8_t mapper_id);
