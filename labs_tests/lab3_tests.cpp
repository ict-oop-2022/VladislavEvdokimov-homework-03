#include <gtest/gtest.h>
#include "lab3/'backup.exe'.cpp"


TEST(CreateBackups, TwoRestorePointsAndThreeStoragesCreated) {
    Repo backup_repo("path/to/file/dir");
    BackupJob job1("Job_3rd_July", backup_repo, "split");
    job1.add("FileA");
    job1.add("FileB");
    job1.start_job();
    job1.remove("FileB");
    job1.start_job();

    vector<RestorePoint> test_points = job1.getRestorePoints();
    assert (test_points.size() == 2);
    assert ((test_points[0].getStorages()).size() == 2);
    assert ((test_points[1].getStorages()).size() == 1);
}


