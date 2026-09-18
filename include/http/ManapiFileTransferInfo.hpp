#pragma once

#include <string>

#include "../fs/ManapiFilesystem.hpp"
#include "./ManapiHttpMime.hpp"

namespace manapi::net::http {
    class file_transfer_info {
    public:
        explicit file_transfer_info (std::string filelocal) {
            this->m_filelocal = std::move(filelocal);
            this->m_filename = fs::path::basename(this->m_filelocal);
            this->m_filemime = mime::mime_by_file_path(this->m_filename);
        }

        file_transfer_info (std::string filelocal, std::string filename, std::string filemime) {
            this->m_filelocal = std::move(filelocal);
            this->m_filename = std::move(filename);
            this->m_filemime = std::move(filemime);
        }

        file_transfer_info (file_transfer_info &&n) MANAPIHTTP_NOEXCEPT {
            this->m_filelocal = std::move(n.m_filelocal);
            this->m_filename = std::move(n.m_filename);
            this->m_filemime = std::move(n.m_filemime);
        }


        file_transfer_info &operator=(file_transfer_info &&n) MANAPIHTTP_NOEXCEPT {
            if (this != &n) {
                this->m_filelocal = std::move(n.m_filelocal);
                this->m_filename = std::move(n.m_filename);
                this->m_filemime = std::move(n.m_filemime);
            }
            return *this;
        }

        std::string &filelocal () {
            return this->m_filelocal;
        }

        std::string &filename () {
            return this->m_filename;
        }

        std::string &filemime () {
            return this->m_filemime;
        }

    private:
        std::string m_filelocal;
        std::string m_filename;
        std::string m_filemime;
    };
}