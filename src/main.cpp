#include <iostream>
#include <getopt.h>
#include <filesystem>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <map>

#include <cutpack/data/cutpack_operation.h>
#include <cutpack/data/cutscene_file.h>
#include <cutpack/util/qb_crc.h>

void printHelp(char *name);

void extractCutscene(const std::string &input, const std::string &output, const std::string &checksumTable, bool verbose);

void packCutscene(const std::string &input, const std::basic_string<char> &output, bool verbose);

int main(int argc, char *argv[]) {
    CutpackOperation operation = UNDEFINED;
    bool verbose;

    std::string input;
    std::string output;
    std::string checksumTable;

    int c;

    if (argc == 1) {
        printHelp(argv[0]);
        return 0;
    }

    while ((c = getopt(argc, argv, "hxpvt:i:o:")) != -1) {
        switch (c) {
            case 'h':
                printHelp(argv[0]);
                return 0;
            case 'x':
                operation = EXTRACT;
                break;
            case 'p':
                operation = PACK;
                break;
            case 'v':
                verbose = true;
                break;
            case 't':
                checksumTable = std::string(optarg);
                break;
            case 'i':
                input = std::string(optarg);
                break;
            case 'o':
                output = std::string(optarg);
                break;
            default:
                printHelp(argv[0]);
                return 0;
        }
    }

    if (operation == UNDEFINED) {
        printf("Please, set -x (extract) or -p (pack) flag UwU\n");
        return 1;
    }

    if (input.empty()) {
        printf("Please, provide an input file or directory UwU");
        return 1;
    }

    printf("Action: %s\n", operation == EXTRACT ? "Extract" : "Pack");
    printf("Input: %s\n", input.empty() ? "[Not Specified]" : input.c_str());
    printf("Output: %s\n\n", output.empty() ? "[Not Specified]" : output.c_str());

    if (operation == EXTRACT) {
        if(output.empty()) {
            output = input + "_data";

            if(verbose) {
                printf("Output directory isn't specified, extracting into %s\n", output.c_str());
            }
        }

        extractCutscene(input, output, checksumTable, verbose);
    }

    if (operation == PACK) {
        packCutscene(input, output, verbose);
    }

    return 0;
}

void extractCutscene(const std::string &input, const std::string &output, const std::string &checksumTable, bool verbose) {
    std::map<uint32_t, std::string> checksums;

    if (!std::filesystem::is_regular_file(input)) {
        printf("Input is not a file!\n");
        return;
    }

    if (std::filesystem::exists(output)) {
        printf("Output directory exists!\n");
        return;
    }

    if(std::filesystem::exists(checksumTable)) {
        std::ifstream checksumTableFile(checksumTable);

        std::string line;

        while(std::getline(checksumTableFile, line)) {
            if(line.empty() || line.rfind("//", 0) == 0) {
                continue;
            }

            size_t pos = line.find(';');

            if(pos != std::string::npos) {
                std::string name = line.substr(0, pos);
                std::string checksum = line.substr(pos + 1);

                try {
                    checksums.insert({std::stoul(checksum, nullptr, 16), name});
                } catch (std::exception& e) {
                    printf("Failed to read checksums table!");
                    checksums.clear();
                    break;
                }
            }
        }
    }

    std::filesystem::create_directory(output);

    std::ifstream cutscene(input, std::ios::binary);

    std::streampos size;

    cutscene.seekg(0, std::ios::end);
    size = cutscene.tellg();
    cutscene.seekg(0, std::ios::beg);

    if (size < 12) {
        printf("The cutscene is too small! Nothing to extract :<\n");
        return;
    }

    uint32_t version;

    cutscene.read(reinterpret_cast<char *>(&version), sizeof(version));

    if (version != 1) {
        printf("Invalid cutscene version!\n");
        return;
    }

    int32_t filesCount;

    cutscene.read(reinterpret_cast<char *>(&filesCount), sizeof(filesCount));

    if (filesCount <= 0) {
        printf("Invalid files count!");
        return;
    }

    if (verbose) {
        printf("--General cutscene info--\n");
        printf("Cutscene version: %d\n", version);
        printf("Files count: %d\n\n", filesCount);
    }

    std::vector<CutsceneFile> files;

    for (int i = 0; i < filesCount; i++) {
        CutsceneFile cutsceneFile = CutsceneFile();

        cutscene.read(reinterpret_cast<char *>(&cutsceneFile.offset), sizeof(cutsceneFile.offset));
        cutscene.read(reinterpret_cast<char *>(&cutsceneFile.size), sizeof(cutsceneFile.size));
        cutscene.read(reinterpret_cast<char *>(&cutsceneFile.name), sizeof(cutsceneFile.name));
        cutscene.read(reinterpret_cast<char *>(&cutsceneFile.extension), sizeof(cutsceneFile.extension));

        files.push_back(cutsceneFile);
    }

    for (CutsceneFile file: files) {
        char *fileData = new char[file.size];

        cutscene.seekg(file.offset, std::ios::beg);

        cutscene.read(fileData, (std::streamsize) file.size);

        std::filesystem::path finalPath = output;
        std::stringstream finalFilename;

        if(checksums.count(file.name)) {
            finalFilename << checksums[file.name];
        } else {
            finalFilename << "0x" << std::setw(8) << std::setfill('0') << std::hex << file.name;
        }

        finalFilename << ".";

        if(checksums.count(file.extension)) {
            finalFilename << checksums[file.extension];
        } else {
            finalFilename << "0x" << std::setw(8) << std::setfill('0') << std::hex << file.extension;
        }
        finalPath /= finalFilename.str();

        std::ofstream outputFile(finalPath, std::ios::binary);

        outputFile.write(fileData, (std::streamsize) file.size);
        outputFile.close();

        printf("%s -> %s\n", finalFilename.str().c_str(), finalPath.c_str());
    }

    cutscene.close();

    printf("%d files have been extracted successfully!\n", filesCount);
}

void packCutscene(const std::string &input, const std::string &output, bool verbose) {
    if (!std::filesystem::is_directory(input)) {
        printf("Input should be a directory!\n");
        return;
    }

    if (std::filesystem::exists(output)) {
        printf("Output file already exists!\n");
        return;
    }

    if(output.empty()) {
        printf("Please, specify an output file!\n");
        return;
    }

    std::vector<std::filesystem::path> files;

    // Iterate over every file and collect it
    for (const auto &file: std::filesystem::directory_iterator(input)) {
        if (file.is_directory()) {
            continue;
        }

        std::string filename = file.path().filename().string();
        files.push_back(file);
    }

    // Sort files by their checksums
    std::sort(files.begin(), files.end(), [](const std::filesystem::path &first, const std::filesystem::path &second) {
        unsigned long firstChecksum;
        unsigned long secondChecksum;

        if (first.filename().string().rfind("0x", 0) == 0) {
            firstChecksum = std::stoul(first.filename().string(), nullptr, 16);
        } else {
            firstChecksum = QbCrc::generate(first.filename().string());
        }

        if (second.filename().string().rfind("0x", 0) == 0) {
            secondChecksum = std::stoul(second.filename().string(), nullptr, 16);
        } else {
            secondChecksum = QbCrc::generate(second.filename().string());
        }

        return firstChecksum < secondChecksum;
    });

    // Read every file, write into the data buffer and store entry in cutscene files vector
    std::vector<CutsceneFile> cutsceneFiles;
    std::vector<char> data;

    uint32_t offset = 0;

    for (const std::filesystem::path &file: files) {
        CutsceneFile cutsceneFile = CutsceneFile();

        if (!std::filesystem::exists(file)) {
            printf("File %s doesn't exist!\n", file.string().c_str());
            continue;
        }

        // Convert name to integer from hex or calculate checksum
        if (file.filename().string().rfind("0x", 0) == 0) {
            cutsceneFile.name = std::stoul(file.filename().string(), nullptr, 16);
        } else {
            cutsceneFile.name = QbCrc::generate(file.filename().string());
        }

        std::string extension = file.extension().string();

        // Remove dot from extension
        extension.erase(extension.begin());

        // Convert extension to integer from hex or calculate checksum
        if (extension.rfind("0x", 0) == 0) {
            cutsceneFile.extension = std::stoul(extension, nullptr, 16);
        } else {
            cutsceneFile.extension = QbCrc::generate(extension);
        }

        // Open the file
        std::ifstream fileData(file, std::ios::binary);

        fileData.seekg(0, std::ios::end);
        cutsceneFile.size = fileData.tellg();
        fileData.seekg(0, std::ios::beg);

        // Read data into the vector
        std::vector<char> tempData(cutsceneFile.size);
        fileData.read(tempData.data(), (std::streamsize) cutsceneFile.size);

        // Close the file
        fileData.close();

        // Insert file into the cutscene
        data.insert(data.end(), tempData.begin(), tempData.end());

        // Print current file, if verbose
        if (verbose) {
            printf("%s -> %s\n", file.string().c_str(), output.c_str());
        }

        // Add cutscene file entry to the vector
        cutsceneFile.offset = offset;
        cutsceneFiles.push_back(cutsceneFile);

        // Increase offset
        offset += cutsceneFile.size;

        // Perform data alignment, if needed
        uint32_t misalignment = data.size() % 4;

        if (misalignment != 0) {
            for (int i = 0; i < 4 - misalignment; i++) {
                data.push_back(0x00);
                offset++;
            }
        }
    }

    std::ofstream cutscene(output, std::ios::out | std::ios::binary);

    uint32_t version = 1;
    uint32_t filesCount = cutsceneFiles.size();

    // Write version and files count to the cutscene
    cutscene.write(reinterpret_cast<const char *>(&version), sizeof(version));
    cutscene.write(reinterpret_cast<const char *>(&filesCount), sizeof(filesCount));

    // Write file entries to the cutscene
    for (CutsceneFile cutsceneFileEntry: cutsceneFiles) {
        cutsceneFileEntry.offset += 8 + (16 * filesCount);

        cutscene.write(reinterpret_cast<const char *>(&cutsceneFileEntry.offset), sizeof(cutsceneFileEntry.offset));
        cutscene.write(reinterpret_cast<const char *>(&cutsceneFileEntry.size), sizeof(cutsceneFileEntry.size));
        cutscene.write(reinterpret_cast<const char *>(&cutsceneFileEntry.name), sizeof(cutsceneFileEntry.name));
        cutscene.write(reinterpret_cast<const char *>(&cutsceneFileEntry.extension), sizeof(cutsceneFileEntry.extension));
    }

    // Write data to the cutscene
    cutscene.write(data.data(), (std::streamsize) data.size());

    // Close the cutscene file
    cutscene.close();

    printf("%d files have been packed successfully!\n", filesCount);
}

void printHelp(char *name) {
    printf("Heyo! This is cutpack, a tool for repacking cutscenes UwU\n");
    printf("Usage: %s [OPTION]\n\n", name);
    printf("Warning: it works only with Tony Hawk's Underground 1/2 .cut/.cut.xbx files (for now)\n");
    printf("Extracts into a directory with the same name if -o is not specified\n\n");
    printf("  -x        Extract cutscene\n");
    printf("  -p        Pack cutscene\n");
    printf("  -t        Checksum table path\n");
    printf("  -i        Input path\n");
    printf("  -o        Output path\n");
    printf("  -v        Verbose\n\n");
    printf("Example: %s -x -i BE_LevelEvent.cut.xbx -o BE_LevelEvent\n", name);
}