#pragma once
#include <iostream>
#include <string>
#include <optional>
#include <utility>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;


class Storage{
protected:
    vector<string> files;
    string storage_name;

public:
    Storage(const vector<string> &files,
            const string &storageName) : files(files), storage_name(storageName) {}

    [[nodiscard]] const vector<string> &getFiles() const {
        return files;
    }

    [[nodiscard]] const string &getStorageName() const {
        return storage_name;
    }
};

class RestorePoint {
protected:
    vector<Storage> storages;

    static vector<Storage> fill_storages(const vector<string>& files,
                                         map<string, int> file_versions,
                                         const string& storage_type);
public:
    RestorePoint(vector<string> files,
                 map<string, int> file_versions,
                 string storage_type) {
        this->storages=fill_storages(std::move(files),
                                     std::move(file_versions),
                                     std::move(storage_type));
    }

    [[nodiscard]] const vector<Storage> &getStorages() const;
};


class Repo {
public:
    string path;

    Repo(string path) : path(std::move(path)) {}

    void save_files(const RestorePoint& point, const string& storage_type);

    static void copy_files(const string& path_from, const string& path_to);
};

class BackupJob {
public:
    string name;

    BackupJob(string name,
              Repo &repo,
              const string &storageType = "single") : name(std::move(name)),
                                                      repo(repo),
                                                      storage_type(storageType) {
        check_storage_type(storageType);
    }

protected:
    Repo &repo;
    string storage_type = "single";
    map<string, int> file_versions;
    vector<string> staged_files;
    vector<RestorePoint> restore_points;

    void update_files_version();

public:
    [[nodiscard]] const string &getStorageType() const;

    [[nodiscard]] const string &getRepo() const;


    [[nodiscard]] const vector<RestorePoint> &getRestorePoints() const;

    [[nodiscard]] const vector<string> &getStagedFiles() const;


    void add(const string& filename);

    void remove(const string& filename);

    void start_job();

protected:
    static void check_storage_type(const string& storage_type);
};