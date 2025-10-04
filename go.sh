

phpize
./configure --enable-fnbind
make clean
make install -j10
make test