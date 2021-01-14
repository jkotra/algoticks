import zipfile
import subprocess
import shutil
import os


def main():

    if os.path.exists("../bin"):
        shutil.rmtree("../bin/")
    
    #delete bin folder
    os.mkdir("../bin/")
    
    #change current dir
    os.chdir("../bin/")
    
    cmake_cmd = "cmake -G \"MSYS Makefiles\" -DCMAKE_BUILD_TYPE=release .."
    print(cmake_cmd)

    subprocess.run(cmake_cmd, shell=True)
    subprocess.run("make")

    filename = "algoticks_{}_windows_x86_64"
    
    
    v = input("algoticks version? (example: 1.0.0)")
    filename = filename.format(v)

    print(filename + ".zip")
    input("Does this look right? press any key..\n Ctrl+C to Abort.")

    shutil.make_archive(filename, 'zip', "release")


if __name__ == "__main__":
    main()