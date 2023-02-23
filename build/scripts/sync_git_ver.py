import os

fd = open("git_version.txt")
dataList = fd.readlines()
rootdir = os.getcwd()
for i in range(0, len(dataList), 3):
    os.chdir("{location}".format(location=dataList[i].split(' ')[1].strip()))
    os.system("git reset --hard {commit}".format(
        commit=dataList[i + 1].split(' ')[0])
    )
    os.chdir(rootdir)
fd.close()
