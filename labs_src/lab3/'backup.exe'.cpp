#include "lab3_functions.h"
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>

using namespace std;

void BackupJob::check_storage_type(const string& StorageType) {
    vector<string> allowed_storage_values = {"single", "split"};

    if (std::find(allowed_storage_values.begin(),
                  allowed_storage_values.end(),
                  StorageType) != allowed_storage_values.end()) {
    } else {
        throw std::invalid_argument("'storage_type' value is wrong one. Use 'single' or 'split'.");
    };
}

void BackupJob::add(const string& filename) {
    staged_files.push_back(filename);
}

void BackupJob::remove(const string& filename) {
    auto itr = std::find(staged_files.begin(), staged_files.end(), filename);
    if (itr != staged_files.end()) {
        staged_files.erase(itr);
    } else {
        cout << "Value " << filename << " is not in staged files." << endl;
        throw invalid_argument("Filename is not in BackupJob staged files.");

    };
}

void BackupJob::start_job() {
    update_files_version();
    RestorePoint point(staged_files,
                       file_versions,
                       storage_type);
    restore_points.push_back(point);
}

const string &BackupJob::getRepo() const {
    return repo.path;
}

const string &BackupJob::getStorageType() const {
    return storage_type;
}

const vector<RestorePoint> &BackupJob::getRestorePoints() const {
    return restore_points;
}

const vector<string> &BackupJob::getStagedFiles() const {
    return staged_files;
}

void BackupJob::update_files_version() {
    for (auto & staged_file : staged_files) {
        if (file_versions.find(staged_file) == file_versions.end()) {
            file_versions.insert({staged_file, 1});
        } else {
            file_versions[staged_file] = file_versions[staged_file] + 1;
        }
    }


}


void Repo::save_files(const RestorePoint& point, const string& storage_type) {
    if (!(std::filesystem::exists(path))) {
        cout << "Path " << path << " does not exist." << endl;
        throw std::invalid_argument("Path does not exist.");
    };
    if (storage_type == "split") {
        vector<Storage> storages = point.getStorages();
        for (auto & storage : storages) {
            string path_storage = path + storage.getStorageName();
            string cmd = "mkdir " + path_storage;
            system(cmd.c_str());
            string file_to_save = storage.getFiles()[0];
            string base_filename_ = file_to_save.substr(file_to_save.find_last_of("/\\") + 1);
            string where_save = path_storage + "\\" + base_filename_;

            copy_files(file_to_save, where_save);
        }
    } else if (storage_type == "single") {
        Storage storage = point.getStorages()[0];
        string path_storage = path + storage.getStorageName();
        string cmd = "mkdir " + path_storage;
        system(cmd.c_str());

        vector<string> files_to_save = storage.getFiles();

        for (auto & i : files_to_save) {
            string base_filename_ = i.substr(i.find_last_of("/\\") + 1);
            string where_save = path_storage + "\\" + base_filename_;
            copy_files(i, where_save);
        }


    }
}

void Repo::copy_files(const string& path_from, const string& path_to) {
    string line;
    ifstream ini_file{path_from};
    ofstream out_file{path_to};

    if (ini_file && out_file) {

        while (getline(ini_file, line)) {
            out_file << line << "\n";
        }

        cout << "Copy Finished \n";

    } else {
        printf("Cannot read File");
    }

    ini_file.close();
    out_file.close();

}


const vector<Storage> &RestorePoint::getStorages() const {
    return storages;
}

vector<Storage> RestorePoint::fill_storages(const vector<string>& files,
                                            map<string, int> file_versions,
                                            const string& storage_type) {
    vector<Storage> storages;

    if (storage_type == "split") {
        for (auto & file : files) {
            string base_filename = file.substr(file.find_last_of("/\\") + 1);
            string storage_name = base_filename + "_" + to_string(file_versions[file]);
            vector<string> storage_files = {file};
            Storage st(storage_files, storage_name);
            storages.push_back(st);
        }
    } else {
        string storage_name = "";
        for (auto & file : files) {
            string base_filename = file.substr(file.find_last_of("/\\") + 1);
            string storage_name_res = storage_name + base_filename + "_" + to_string(file_versions[file]) + "+";
            storage_name = storage_name_res;
        }
        Storage st(files, storage_name);
        storages.push_back(st);
    };
    return storages;
}





