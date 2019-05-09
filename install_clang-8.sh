echo "I totally didn't test this so you might have to fix it. Good news is that it probably won't destroy your life"
set -x
mkdir ~/requirements
cd ~/requirements
wget http://releases.llvm.org/7.0.1/clang+llvm-7.0.1-x86_64-linux-gnu-ubuntu-16.04.tar.xz
tar -xvf clang+llvm-7.0.1-x86_64-linux-gnu-ubuntu-16.04.tar.xz
echo "export PATH=$PATH:~/requirements/clang+llvm-8.0.0-x86_64-linux-gnu-ubuntu-16.04/bin" >> ~/.bash_profile
