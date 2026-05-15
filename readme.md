## Systemy wbudowane i sterowniki - projekt

Projekt polegający na napisaniu sterownika i aplikacji do akceleratora PKA (Public Key Accelerator)
emulowanego w qemu. Model akceleratora dostępny jest w submodule qemu.

### Uruchomienie

Pobierz BR, wygeneruj konfigurację i uruchom kompilację:
```
./bootstrap
cd buildroot && make defconfig BR2_DEFCONFIG=../defconfig
make
``` 

Pobierz/zaktualizuj zawartość w submodule:
```
git submodule update --init
```

Skonfiguruj system budowania w qemu i uruchom kompilację:
```
cd qemu && ./setup
make -C ./build -j $(nproc)
```

Uruchom maszynę wraz z emulowanym akceleratorem:
```
./qemu/build/qemu-system-aarch64 -M virt -cpu cortex-a53 -nographic -smp 1 \
-kernel ./buildroot/output/images/Image -append "loglevel=2" \
-virtfs local,path=./shared,mount_tag=shared_folder,security_model=mapped \
-device pka
```

### Użytkowanie

Sprawdź numery szyny i urządzenia PCIe oraz adres bazowy pierwszego regionu zmapowanej przestrzeni
adresowej urządzenia (BAR0), i na ich podstawie ustaw bit umożliwiający dostęp tej przestrzeni
adresowej:
```
lspci
setpci <nr_szyny:nr_urządzenia> command=2
```

Odczytaj pierwszy rejestr, w którym zawarte jest ID urządzenia oraz zapisz wartość do drugiego
rejestru i sprawdź poprawność zapisu:
```
devmem <adres_bazowy_regionu0>
devmem <adres_bazowy_regionu0 + 4>
devmem <adres_bazowy_regionu0 + 4> 32 1
devmem <adres_bazowy_regionu0 + 4>
```

Zamontuj współdzielony katalog:
```
mkdir -p /mnt/shared && mount -t 9p -o trans=virtio,version=9p2000.L shared_folder /mnt/shared
```
