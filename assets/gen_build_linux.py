import tarfile
import subprocess
import shutil
import os


def main():

    if os.path.exists("../bin"):
        shutil.rmtree("../bin/")
    
    #delete bin folder
    os.mkdir("../bin/")

    os.chdir("../bin/")
    subprocess.run("cmake -DCMAKE_BUILD_TYPE=Release ..".split())
    subprocess.run("make")

    filename = "algoticks_{}_linux_x86-64.tar.xz"

    version = subprocess.Popen("./release/algoticks -V".split(), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    v, err = version.communicate()
    v = v.split()[1].decode("utf-8")

    filename = filename.format(v)
    print(filename)
    
    os.chdir("release")
    rls = tarfile.open(filename, mode="w")

    for file in os.listdir("."):
        rls.add(file)

    rls.close()


if __name__ == "__main__":
    main()