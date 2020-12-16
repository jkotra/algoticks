import zipfile
import subprocess
import shutil
import os


def main():

    if os.path.exists("../bin"):
        shutil.rmtree("../bin/")
    
    #delete bin folder
    os.mkdir("../bin/")

    os.chdir("../bin/")
    
    cmake_cmd = "cmake -G \"MSYS Makefiles\" -DCMAKE_BUILD_TYPE=release .."
    print(cmake_cmd)

    subprocess.run(cmake_cmd, shell=True)
    subprocess.run("make")

    filename = "algoticks_{}_windows_x86_64"
    
    
    v = input("algoticks version?")
    filename = filename.format(v)
    print(filename + ".zip")
    input("Does this look right?")

    shutil.make_archive(filename, 'zip', "release")


if __name__ == "__main__":
    main()