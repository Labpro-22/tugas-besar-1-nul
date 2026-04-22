# Spesifikasi Tugas Besar 1: Nimonspoli

## IF2010 Pemrograman Berorientasi Objek

Version history:
Revisi 1: 06 April 2026 - Perubahan deadline M
Revisi 2: 22 April 2026 - Penambahan konfigurasi PETAK_AKSI


## Daftar Isi........................................................................................................................................................................

## Daftar Isi


- Daftar Isi........................................................................................................................................................................
- Deskripsi Persoalan...................................................................................................................................................
- Alur Program...............................................................................................................................................................
- Konfigurasi...................................................................................................................................................................
- Mekanisme...................................................................................................................................................................
   - Papan Permainan..........................................................................................................................................................
      - PETAK PROPERTI................................................................................................................................................
      - PETAK AKSI............................................................................................................................................................
   - Petak Spesial................................................................................................................................................................
   - Dadu dan Giliran Bermain......................................................................................................................................
   - Kepemilikan Properti..............................................................................................................
      - Street (e.g., BDG, DEN, MTR, dst.).................................................................................
      - Railroad (e.g., TUG, GUB)...............................................................................................
      - Utility (PLN dan PAM)......................................................................................................
   - Ketentuan Sewa....................................................................................................................
   - Gadai.....................................................................................................................................
   - Peningkatan Properti...............................................................................................................................................
   - Lelang Tanah.................................................................................................................................................................
      - Mekanisme Lelang.............................................................................................................................................
   - Pajak................................................................................................................................................................................
   - Festival...........................................................................................................................................................................
   - Kartu Kesempatan dan Dana Umum.................................................................................................................
      - Kartu Kesempatan.............................................................................................................................................
      - Kartu Dana Umum.............................................................................................................................................
   - Kartu Kemampuan Spesial.....................................................................................................................................
      - Aturan Kartu Kemampuan Spesial..............................................................................................................
      - Jenis Kartu Kemampuan Spesial..................................................................................................................
   - Bangkrut........................................................................................................................................................................
      - Mekanisme Likuidasi Aset...............................................................................................
   - Pengambilalihan Asset.............................................................................................................................................
   - Kondisi Game Selesai...............................................................................................................................................
      - MAX TURN...........................................................................................................................................................
      - BANKRUPTCY....................................................................................................................................................
   - Save/Load......................................................................................................................................................................
   - Transaction Logger....................................................................................................................................................
- Operasi dan Perintah..............................................................................................................................................
   - 1. CETAK_PAPAN.................................................................................................................
   - 2. LEMPAR_DADU................................................................................................................
   - 3. ATUR_DADU X Y..............................................................................................................
   - 4. CETAK_AKTA..........................................................................................................................................................
   - 5. CETAK_PROPERTI...............................................................................................................................................
   - 6. BELI (OTOMATIS).................................................................................................................................................
   - 7. BAYAR_SEWA (OTOMATIS).............................................................................................................................
   - 8. BAYAR_PAJAK (OTOMATIS)............................................................................................................................
   - 9. GADAI........................................................................................................................................................................
   - 10. TEBUS.....................................................................................................................................................................
   - 11. BANGUN...............................................................................................................................................................
   - 12. LELANG (OTOMATIS).....................................................................................................
   - 13. FESTIVAL (OTOMATIS)..................................................................................................
   - 14. BANGKRUT DAN AMBIL ALIH ASET (OTOMATIS).......................................................
   - 15. SIMPAN...........................................................................................................................
   - 16. MUAT...............................................................................................................................
   - 17. CETAK_LOG....................................................................................................................
   - 18. MENANG (OTOMATIS)...................................................................................................
   - 19. KESEMPATAN DAN DANA UMUM (OTOMATIS)...........................................................
   - 20. GUNAKAN KEMAMPUAN...............................................................................................
   - 21. DROP KARTU KEMAMPUAN (OTOMATIS)...................................................................
- Bonus..........................................................................................................................................
   - Antarmuka Pengguna Grafis/Graphical User Interface (GUI).................................................................
   - COM................................................................................................................................................................................
   - Papan Dinamis......................................................................................................................
   - Kreativitas....................................................................................................................................................................
- Spesifikasi Sistem....................................................................................................................................................
   - Ketentuan Umum.......................................................................................................................................................
   - Ketentuan Teknis.......................................................................................................................................................
- Panduan Pengerjaan...............................................................................................................................................
- Milestone Pengumpulan.......................................................................................................................................
   - Milestone
   - Milestone
- Asistensi......................................................................................................................................................................
- QnA..............................................................................................................................................................................


## Deskripsi Persoalan...................................................................................................................................................

Di suatu pagi yang cerah, Gro terbangun dari tidurnya. Dari ruang santai terdengar gelak tawa
yang cukup ramai. Saat menghampiri sumber suara tersebut, Gro mendapati Kebin dan Stewart
sedang asyik memainkan gim konsol terbaru. Namun, suasana segera berubah ketika Gro
menyadari bahwa gim tersebut ternyata dibeli menggunakan akun miliknya tanpa
sepengetahuannya.

Melihat kedua Nimons yang langsung ketakutan dan hampir menangis, amarah Gro perlahan
mereda. Ia justru mendapat sebuah ide. Daripada Kebin dan Stewart terus membeli gim baru yang
menguras isi dompetnya, lebih baik ia menyediakan sendiri sebuah permainan yang tidak kalah
seru untuk mereka mainkan.

Gro pun teringat pada sebuah permainan papan yang pernah ia temui saat berkunjung ke sebuah
_board game café_ bersama istrinya. Permainan itu bernama **Nimonspoli** , sebuah permainan strategi
yang membuat para pemainnya dapat membeli properti, membayar sewa, mengikuti lelang, hingga
mengelola kekayaan sambil berkeliling papan permainan.

Sayangnya, meskipun Gro adalah mantan kriminal hebat yang kini beralih menjadi agen
mata-mata, ia sama sekali tidak memiliki kemampuan untuk mengembangkan gim semacam itu.
Oleh karena itu, Gro membutuhkan bantuan Anda untuk membangun gim **Nimonspoli** dalam
bahasa C++ dengan memanfaatkan konsep-konsep OOP. Bantulah Gro membuat permainan yang
seru agar Kebin, Stewart, dan para Nimons lainnya dapat bermain tanpa kembali diam-diam
menghabiskan uang Gro.


## Alur Program...............................................................................................................................................................

1. Saat program dijalankan, program akan membaca berkas konfigurasi yang memiliki format
    seperti pada Konfigurasi
2. Program akan menampilkan menu dan memberikan 2 opsi untuk menginisiasi state
    program
       a. Load Game
          i. Melakukan _load state game_ berdasarkan Save/Load
ii. Urutan giliran pemain diambil dari state tersimpan di Save/Load
       b. New Game
          i. Melakukan input jumlah pemain (Min: 2 Pemain, Maks: 4)
ii. Menginputkan username masing-masing pemain
iii. Urutan giliran pemain akan diputuskan pada awal permainan melalui
pengacakan random
3. Satu siklus giliran pemain disebut turn. (Pemain 1 → Pemain 2 → ... → Pemain N)
4. Permainan berjalan sesuai dengan Mekanisme permainan
5. Dalam permainan, pemain dapat memilih sebuah perintah dari daftar perintah berikut
    Operasi dan Perintah. Lakukan validasi masukan untuk setiap perintah.
6. Berakhirnya permainan mengikuti aturan Kondisi Game Selesai

## Konfigurasi...................................................................................................................................................................

1. Konfigurasi Property
    Setiap properti terdiri dari ID yang merupakan indeks, kode huruf, nama, blok warna, harga
    beli tanah dasar, harga bangun rumah/hotel dan harga sewa bertingkat mulai dari tanah
    kosong (Level 0) hingga hotel (Level 5). Kode huruf dijamin unik.
       property.txt
       ID KODE NAMA JENIS WARNA HARGA_LAHAN NILAI_GADAI UPG_RUMAH
       UPG_HT RENT_L0...RENT_L
       2 GRT GARUT STREET COKLAT 60 40 20 50 2 ... 250
       4 TSK TASIKMALAYA STREET COKLAT 60 40 50 50 4 ... 320
       6 GBR STASIUN_GAMBIR RAILROAD DEFAULT 0 20
       7 BGR BOGOR STREET BIRU_MUDA 100 80 20 50 6 ... 400
       9 DPK DEPOK STREET BIRU_MUDA 100 80 20 50 6 ... 400
       10 BKS BEKASI STREET BIRU_MUDA 120 90 20 50 8 ... 450
       12 MGL MAGELANG STREET MERAH_MUDA 140 100 100 100 10 ... 750
       13 PLN PLN UTILITY ABU_ABU 0 25
       14 SOL SOLO STREET MERAH_MUDA 140 100 100 100 10 50 ... 750
       15 YOG YOGYAKARTA STREET MERAH_MUDA 160 120 100 100 12 ... 900
       16 STB STASIUN_BANDUNG RAILROAD DEFAULT 0 40
       17 MAL MALANG STREET ORANGE 180 135 100 100 14 ... 950
       19 SMG SEMARANG STREET ORANGE 180 140 100 100 14 ... 950


##### 20 SBY SURABAYA STREET ORANGE 200 150 100 100 16 ... 1000

##### 22 MKS MAKASSAR STREET MERAH 220 175 150 150 18 ... 1050

##### 24 BLP BALIKPAPAN STREET MERAH 220 175 150 150 18 ... 1050

##### 25 MND MANADO STREET MERAH 240 190 150 150 20 ... 1100

##### 26 TUG STASIUN_TUGU RAILROAD DEFAULT 0 50

##### 27 PLB PALEMBANG STREET KUNING 260 200 150 150 22 ... 1150

##### 28 PKB PEKANBARU STREET KUNING 260 210 150 150 22 ... 1150

##### 29 PAM PAM UTILITY ABU_ABU 0 60

##### 30 MED MEDAN STREET KUNING 280 225 150 150 24 ... 1200

##### 32 BDG BANDUNG STREET HIJAU 300 250 200 200 26 ... 1275

##### 33 DEN DENPASAR STREET HIJAU 300 260 200 200 26 ... 1275

##### 35 MTR MATARAM STREET HIJAU 320 280 200 200 28 ... 1400

##### 36 GUB STASIUN_GUBENG RAILROAD DEFAULT 0 120

##### 38 JKT JAKARTA STREET BIRU_TUA 350 300 200 200 35 ... 1500

##### 40 IKN IBU_KOTA_NUSANTARA STREET BIRU_TUA 400 350 200 200 50 ...

##### 2000

2. Konfigurasi Fasilitas Publik
    a. Railroad
       Setiap railroad memiliki biaya sewa yang bergantung pada banyaknya railroad yang
       dimiliki oleh pemilik petak. Semakin banyak railroad yang dimiliki oleh pemain,
       semakin besar biaya sewa yang harus dibayarkan oleh pemain lain yang mendarat
       di petak railroad miliknya.
          railroad.txt
          JUMLAH_RAILROAD BIAYA_SEWA
          1 25
          2 50
          3 100
          4 200

```
b. Utility
Setiap utility memiliki biaya sewa dihitung dari total angka dadu yang dilempar
pemain yang mendarat, dikalikan faktor pengali sesuai jumlah utility yang dimiliki
pemilik petak. Semakin banyak utility yang dimiliki oleh pemain, semakin besar
faktor pengalinya.
utility.txt
JUMLAH_UTILITY FAKTOR_PENGALI
1 4
2 10
```
3. Konfigurasi Pajak


```
Konfigurasi pajak terdiri dari besar pembayaran flat Pajak Penghasilan (PPH), persentase
pembayaran Pajak Penghasilan terhadap total kekayaan pemain, serta besar Pajak Barang
Mewah (PBM).
tax.txt
PPH_FLAT PPH_PERSENTASE PBM_FLAT
150 10 200
```
4. Konfigurasi Petak Aksi
    Konfigurasi ini berfungsi untuk pemetaan informasi PETAK AKSI ke dalam map Nimonspoli
       aksi.txt
       ID KODE NAMA JENIS_PETAK WARNA
       1 GO Petak_Mulai SPESIAL DEFAULT
       3 DNU Dana_Umum KARTU DEFAULT
       5 PPH Pajak_Penghasilan PAJAK DEFAULT
       8 FES Festival FESTIVAL DEFAULT
       11 PEN Penjara SPESIAL DEFAULT
       18 DNU Dana_Umum KARTU DEFAULT
       21 BBP Bebas_Parkir SPESIAL DEFAULT
       23 KSP Kesempatan KARTU DEFAULT
       31 PPJ Petak_Pergi_ke_Penjara SPESIAL DEFAULT
       34 FES Festival FESTIVAL DEFAULT
       37 KSP Kesempatan KARTU DEFAULT
       39 PBM Pajak_Barang_Mewah PAJAK DEFAULT
5. Konfigurasi Petak Spesial
    Konfigurasi petak spesial terdiri dari besaran gaji yang diterima pemain saat berhenti tepat
    di petak GO atau melewatinya, serta besaran denda yang harus dibayarkan pemain untuk
    keluar dari penjara.
       special.txt
       GO_SALARY JAIL_FINE
       200 50
6. Konfigurasi Lain Lain
    Konfigurasi lain-lain terdiri dari konfigurasi jumlah maksimal giliran bermain dan saldo tiap
    pemain di awal permainan dimulai.
       misc.txt
       MAX_TURN SALDO_AWAL


##### 15 1000

## Mekanisme...................................................................................................................................................................

### Papan Permainan..........................................................................................................................................................

Permainan Nimonspoli dilaksanakan dalam sebuah papan permainan yang berisi dari 40 petak
membentuk persegi. Dalam papan permainan Nimonspoli terdapat 2 tipe petak, yaitu petak
properti dan petak aksi.


```
Gambar 1. Contoh Papan Permainan Nimonspoli
```
#### PETAK PROPERTI................................................................................................................................................

Terdapat 28 **Properti** dalam permainan, dengan masing-masing properti memiliki kartu sertifikat
yang merinci **Harga Sewa** , **Biaya Pembangunan,** serta **Nilai Gadai** (untuk detailnya bisa dilihat di
Kepemilikan Properti). Properti tersebut dibagi sebagai berikut:
**● Petak Lahan** adalah properti paling banyak dengan total jumlah **22 lahan** dalam papan
permainan. Setiap petak lahan termasuk dalam sebuah set warna yang berisi total 2/
lahan. Mulai dari **petak MULAI** dan bergerak _clockwise_ , set warna adalah **cokelat** , **biru
muda** , **pink** , **orange** , **merah** , **kuning** , **hijau** , dan **biru tua**. Set cokelat dan biru tua hanya
terdiri dari 2 lahan dan sisanya 3 lahan.


```
● Petak Stasiun
Petak stasiun adalah petak yang terdapat di tengah pada setiap sisinya sehingga total
jumlahnya ada 4.
● Petak Utilitas
Petak utilitas terdiri dari 2 petak, PLN dan PAM.
```
#### PETAK AKSI............................................................................................................................................................

Terdapat 12 petak aksi pada papan yang terdiri dari:
● Empat (4) **Petak Kartu** yang terbagi menjadi Kartu Kesempatan dan Dana Umum
● Dua (2) **Petak Festival**
● Dua (2) **Petak Pajak**
● Empat (4) **Petak Spesial** yang terdapat di ujung papan permainan.

Berikut merupakan tabel urutan petak dan gambar papan permainan Nimonspoil:

```
Indeks Kode Nama Lengkap Petak Tipe Kategori/Warna
1 GO Petak Mulai Petak Spesial Default
```
```
2 GRT Garut Petak Lahan Coklat
3 DNU Dana Umum Petak Kartu Default
```
```
4 TSK Tasikmalaya Petak Lahan Coklat
5 PPH Pajak Penghasilan Petak Pajak Default
```
```
6 GBR Stasiun Gambir Petak Stasiun Default
7 BGR Bogor Petak Lahan Biru Muda
```
```
8 FES Festival Petak Festival Default
9 DPK Depok Petak Lahan Biru Muda
10 BKS Bekasi Petak Lahan Biru Muda
```
```
11 PEN Penjara Petak Spesial Default
12 MGL Magelang Petak Lahan Merah Muda (Pink)
```
```
13 PLN PLN Petak Utilitas Abu-abu
14 SOL Solo Petak Lahan Merah Muda (Pink)
```
```
15 YOG Yogyakarta Petak Lahan Merah Muda (Pink)
16 STB Stasiun Bandung Petak Stasiun Default
```

```
17 MAL Malang Petak Lahan Orange
18 DNU Dana Umum Petak Kartu Default
```
```
19 SMG Semarang Petak Lahan Orange
20 SBY Surabaya Petak Lahan Orange
```
```
21 BBP Bebas Parkir Petak Spesial Default
22 MKS Makassar Petak Lahan Merah
```
```
23 KSP Kesempatan Petak Kartu Default
24 BLP Balikpapan Petak Lahan Merah
25 MND Manado Petak Lahan Merah
```
```
26 TUG Stasiun Tugu Petak Stasiun Default
27 PLB Palembang Petak Lahan Kuning
```
```
28 PKB Pekanbaru Petak Lahan Kuning
29 PAM PAM Petak Utilitas Abu-abu
```
```
30 MED Medan Petak Lahan Kuning
31 PPJ Petak Pergi ke Penjara Petak Spesial Default
```
```
32 BDG Bandung Petak Lahan Hijau
33 DEN Denapasar Petak Lahan Hijau
```
```
34 FES Festival Petak Festival Default
35 MTR Mataram Petak Lahan Hijau
36 GUB Stasiun Gubeng Petak Stasiun Default
```
```
37 KSP Kesempatan Petak Kartu Default
38 JKT Jakarta Petak Lahan Biru Tua
```
```
39 PBM Pajak Barang Mewah Petak Pajak Default
40 IKN Ibu Kota Nusantara Petak Lahan Biru Tua
```
```
Default = Tentukan warna default papan permainan kalian!
```
Warna bisa kalian sesuaikan kembali agar visualisasi lebih enak dilihat, tetapi semua petak tetap


```
harus dalam kategori yang sama
```
+----------|[DF] BBP +----------+|[MR] MKS |----------[DF] KSP +----------+----------+|[MR] BLP |[MR] MND |----------[DF] TUG +----------+----------|[KN] PLB |[KN] PKB +----------|[AB] PAM +----------+|[KN] MED |----------+[DF] PPJ | (^)
|+---------- (1) |P1+----------+ ^ | (^) ----------|P2+----------+----------+ ^^ |P3 ^^^ |P1---------- |P2+----------+---------- ^ |P2 ^ |P4+---------- |P2+----------+ ^^ | (^) ----------+| (^)
|[OR]|P3 * SBY || |[HJ]|P4 ^ BDG || (^)
+----------+|[OR] SMG | (^) ================================== +----------+|[HJ] DEN | (^)
|P3+----------+ ^^ | ||================================== NIMONSPOLI || |P4+----------+ ^ | (^)
|[DF]| DNU || (^) TURN 15 / 50 |[DF]| FES || (^)
+----------+|[OR] MAL | +----------+|[HJ] MTR | (^)
|P3+----------+ ^ | ----------------------------------LEGENDA KEPEMILIKAN & STATUS |P4+----------+ ^^ | (^)
|[DF]|P1 STB || P1-P4^ :: PropertiRumah Level milik 1 Pemain 1-4 |[DF]|P2 GUB || (^)
+----------+|[PK] YOG | ^^^^^ :: RumahRumah LevelLevel 23 +----------+|[DF] KSP | (^)
|P4+----------+ ^^^ | *(1)-(4): : Hotel Bidak(Maksimal) (IN=Tahanan, (^) V=Mampir) |+----------+ | (^)
|[PK]|P4 ^^ SOL || ----------------------------------KODE WARNA: |[BT]|P1 ^ JKT(4)| | (^)
+----------+|[AB] PLN | [CK]=Coklat[BM]=Biru Muda [MR]=Merah[KN]=Kuning +----------+|[DF] PBM | (^)
|P2+----------+ | [PK]=Pink[OR]=Orange [HJ]=Hijau[BT]=Biru Tua |+----------+ | (^)
|[PK]|P4 ^ MGL || [DF]=Aksi [AB]=Utilitas |[BT]|P1 * IKN || (^)
+----------|[DF] PEN +----------+----------+|[BM] BKS |[BM] DPK |----------[DF] FES +----------+|[BM] BGR |----------+----------[DF] GBR |[DF] PPH +----------+|[CK] TSK |----------[DF] DNU +----------+|[CK] GRT |----------+[DF] GO | (^)
|+---------- IN:2 V:3 |P2+----------+----------+ ^ |P2 ^^ | (^) ----------|P2+----------+ ^^^ |P3----------+---------- | |P1+----------+ ^^ | (^) ----------|P1+----------+ ^ | (^) ----------+| (^)
Tampilan di atas hanyalah contoh visualisasi, untuk ketentuan visualisasi papan permainan dapat
dilihat pada bagian Bab Operasi dan Perintah.

### Petak Spesial................................................................................................................................................................

Pada papan permainan Nimonspoli terdapat 4 petak spesial dengan mekanismenya masing-masing
yang terdiri dari:

1. **Petak Mulai (Go)**
    Petak Go adalah titik awal permainan untuk semua permainan. Mekanisme utamanya
    adalah sebagai sumber pendapatan tetap. Setiap kali bidak pemain berhenti tepat di petak
    ini atau melewati petak ini dalam gilirannya, pemain berhak menerima uang dari Bank
    sebesar **gaji yang ditulis di file konfigurasi.
2. Petak Penjara**
    Petak ini terbagi menjadi dua bagian dengan mekanisme yang berbeda tergantung
    bagaimana pemain bisa sampai di petak ini:
       ● Hanya Mampir: Jika pemain berhenti di petak ini dari hasil lemparan dadu normal,
          pemain hanya dianggap sebagai pengunjung. Tidak ada penalti, pemain aman, dan
          pada giliran berikutnya pemain bisa bergerak seperti biasa.
       ● Di Dalam Penjara: Pemain berstatus tahanan jika pemain ke penjara karena
          mendarat di Petak “ **Pergi ke Penjara** “, mendapatkan kartu yang menyuruh masuk
          penjara, atau melempar dadu _double_ tiga kali berturut-turut. Untuk keluar pada
          giliran berikutnya, pemain memiliki tiga pilihan:


```
i. Membayar denda ke Bank sebesar denda yang ditulis di file konfigurasi
sebelum melempar dadu.
ii. Menggunakan kartu “Bebas dari Penjara” (jika punya).
iii. Mencoba melempar dadu dan harus mendapatkan angka double. Jika gagal,
pemain tidak bergerak untuk giliran tersebut. Batas giliran percobaan
dengan melempar dadu adalah 3 giliran. Pada giliran ke-4, pemain wajib
keluar penjara dengan cara (i), yaitu membayar denda ke Bank.
```
**3. Petak Bebas Parkir**
    Petak ini berfungsi sebagai _rest area_. Jika pemain berhenti di sini, tidak ada aksi apa pun
    yang terjadi.
**4. Petak Pergi ke Penjara**
    Petak ini adalah petak jebakan yang terdapat pada papan permainan. Jika pemain
    mendarat di petak ini, pemain harus memindahkan bidak pemain ke **Petak Penjara** dan
    statusnya langsung menjadi tahanan. Giliran pemain berakhir saat itu juga dan pemain
    tidak mendapatkan gaji dari melewati **Petak Mulai**.

### Dadu dan Giliran Bermain......................................................................................................................................

Permainan Nimonspoli menggunakan **dua dadu bersisi 6 (2d6)** untuk menentukan pergerakan
pemain. Dadu dapat dilempar secara _random_ atau ditentukan manual angka tiap dadunya. Berikut
merupakan aturan penggunaan dadu baik ketika di- _random_ maupun ditentukan manual:
● Pemain mengocok kedua dadu dan memajukan bidaknya searah jarum jam sesuai dengan
total angka yang muncul pada kedua dadu tersebut.
● Jika seorang pemain melempar dan mendapatkan angka yang sama pada kedua dadu
(misalnya 6 dan 6), pemain tersebut menyelesaikan aksinya di petak tempat ia pergi, lalu
**berhak mengocok dadu lagi** untuk giliran tambahan.
● Jika pemain mendapatkan angka _double_ sebanyak tiga kali berturut-turut dalam satu
giliran yang sama, pemain tersebut dianggap “melanggar batas kecepatan”. Bidak tidak
digerakkan berdasarkan angka dadu ketiga, tetapi pemain langsung memindahkan
bidaknya ke **Petak Penjara** dan gilirannya berakhir.

### Kepemilikan Properti..............................................................................................................

_(Untuk kemudahan, terms yang digunakan pada bagian ini akan mengacu pada permainan monopoli asli
agar kalian dapat mengacu dengan lebih mudah dari permainan aslinya)_

Properti adalah petak tanah yang dapat dimiliki oleh pemain. Terdapat tiga jenis properti dalam
permainan ini, yaitu Street, Railroad, dan Utility. Setiap properti memiliki Akta Kepemilikan yang
menyimpan informasi berikut dan didefinisikan di file konfigurasi: nama properti dan jenisnya
(Street/Railroad/Utility), _color group_ (khusus Street), harga beli, tabel harga sewa per level
bangunan, nilai gadai, serta harga bangunan per rumah dan hotel (khusus Street).


Setiap properti memiliki salah satu dari tiga status berikut. Status **BANK** berarti properti belum
dimiliki siapapun. Status **OWNED** berarti properti dimiliki oleh salah satu pemain dan sewa
berlaku normal. Status **MORTGAGED** berarti properti dimiliki pemain tetapi sedang digadaikan
sehingga sewa tidak berlaku.

#### Street (e.g., BDG, DEN, MTR, dst.).................................................................................

Street adalah jenis properti yang paling umum dalam permainan. Setiap Street termasuk dalam
sebuah _color group_ , yaitu kelompok properti yang berbagi warna yang sama. Pemain yang berhasil
memiliki seluruh Street dalam satu _color group_ disebut memonopoli _color group_ tersebut, dan
mendapatkan keuntungan berupa sewa yang lebih tinggi serta kemampuan untuk membangun
rumah dan hotel di atasnya.

Ketika pemain mendarat di petak Street berstatus **BANK** , program secara otomatis menampilkan
informasi Akta Kepemilikan dan menawarkan pembelian kepada pemain. Jika pemain memilih
membeli dan uangnya mencukupi, uang pemain dikurangi sebesar harga beli dan status properti
berubah menjadi **OWNED**. Jika pemain memilih tidak membeli atau uang tidak mencukupi,
properti langsung masuk sistem lelang sesuai mekanisme yang berlaku.

#### Railroad (e.g., TUG, GUB)...............................................................................................

Railroad dimiliki oleh pemain **pertama** yang tiba di petak tersebut secara otomatis tanpa perlu
membayar. Tidak ada proses beli maupun lelang. Meskipun cara perolehannya berbeda dengan
Street, Railroad tetap dapat digadaikan, ditebus, dan dijual ke pemain lain. Semakin banyak
Railroad yang dimiliki oleh pemain, otomatis biaya sewa akan semakin tinggi pula (ditentukan
melalui _config_ ).

#### Utility (PLN dan PAM)......................................................................................................

Utility memiliki mekanisme kepemilikan yang sama dengan Railroad. Pemain **pertama** yang tiba di
petak Utility langsung mendapatkan kepemilikannya secara otomatis tanpa perlu membayar, dan
tidak ada proses beli maupun lelang. Stok utilitas bersifat sudah terbangun sejak awal. Artinya
pemain hanya perlu menginjaknya untuk memilikinya.

Perhitungan biaya sewa Utility ditentukan oleh **total angka dadu** yang didapatkan pemain ketika
mendarat, dikalikan dengan faktor pengali yang bergantung pada jumlah petak _Utility_ yang dimiliki
oleh pemiliknya. Semakin banyak Utility yang dimiliki maka faktor pengali akan semakin besar
(faktor pengali dibaca melalui _config_ ). Meskipun cara perolehannya berbeda dengan Street, Utility
tetap dapat digadaikan, ditebus, dan dijual ke pemain lain

### Ketentuan Sewa....................................................................................................................


Ketika pemain mendarat di properti berstatus **OWNED** milik pemain lain, pembayaran sewa
dilakukan secara otomatis dan hasilnya ditampilkan ke layar beserta log transaksi. Jika properti
berstatus **MORTGAGED** , tidak ada sewa yang dikenakan.

Besaran sewa bergantung pada jenis properti:

1. **Street:** Besaran sewa bergantung pada jumlah bangunan yang ada dan status monopoli
    _color group_ , dengan nilai yang didefinisikan di file konfigurasi per properti. Apabila pemain
    memonopoli seluruh Street dalam satu _color group_ dan belum ada bangunan yang didirikan,
    sewa yang dikenakan adalah dua kali sewa dasar. Setelah bangunan mulai didirikan, sewa
    mengikuti tabel yang tertera pada Akta Kepemilikan, mulai dari 1 rumah hingga hotel. Jika
    efek Festival aktif pada properti tersebut, nilai sewa yang dikenakan adalah nilai sewa
    terkini setelah penggandaan Festival diterapkan.
2. **Railroad:** Besaran sewa ditentukan oleh jumlah Railroad yang dimiliki oleh pemilik properti
    tersebut, sesuai tabel yang didefinisikan di file konfigurasi.
3. **Utility:** Besaran sewa dihitung dari total angka dadu yang dilempar pemain yang mendarat,
    dikalikan faktor pengali sesuai jumlah Utility yang dimiliki pemiliknya, sebagaimana
    didefinisikan di file konfigurasi.

Apabila pemain tidak mampu membayar sewa, pemain memasuki kondisi kebangkrutan. Alur
kebangkrutan ditangani oleh bagian Kebangkrutan dan Pengambilalihan Aset.

### Gadai.....................................................................................................................................

Pemain dapat menggadaikan properti yang dimilikinya kepada Bank kapan saja selama giliran
mereka. Syarat menggadaikan adalah properti berstatus **OWNED** dan tidak memiliki bangunan
yang berdiri di atasnya. Jika masih ada bangunan pada _color group_ yang sama, semua bangunan
pada seluruh _color group_ tersebut harus dijual ke Bank terlebih dahulu dengan harga setengah dari
harga beli bangunan sebelum properti dapat digadaikan. Penjualan bangunan dan penggadaian
properti dapat dilakukan dalam satu rangkaian aksi **GADAI** yang sama tanpa perlu memanggil
perintah ulang.

Ketika properti berhasil digadaikan, pemain menerima uang sebesar nilai gadai properti tersebut
sesuai yang tertera pada Akta Kepemilikan dan status properti berubah menjadi **MORTGAGED**.

Untuk menebus properti yang sedang digadaikan, pemain membayar Bank sebesar **harga beli
penuh** properti tersebut dan status properti kembali menjadi **OWNED**. Properti berstatus
**MORTGAGED** tidak dapat menghasilkan pendapatan sewa selama belum ditebus.

Properti berstatus **MORTGAGED tidak dapat** langsung dijual ke Bank dalam proses likuidasi.
Pemain harus menebus properti tersebut terlebih dahulu (membayar harga beli penuh) sebelum
dapat menjualnya kembali ke Bank dengan harga beli penuh. Oleh karena itu, dalam kondisi
bangkrut, menggadaikan properti umumnya lebih menguntungkan daripada menebus lalu menjual.


### Peningkatan Properti...............................................................................................................................................

Pemain dapat meningkatkan properti jenis Street yang dimilikinya dengan membangun rumah dan
hotel selama giliran mereka. Peningkatan properti hanya dapat dilakukan apabila pemain telah
memonopoli seluruh Street dalam satu _color group_. Setiap peningkatan akan menaikkan nilai sewa
yang harus dibayar oleh pemain lain yang mendarat di properti tersebut.

**Stok bangunan tidak terbatas**. Pemain dapat membangun rumah dan hotel tanpa dibatasi oleh
ketersediaan stok fisik.

Terdapat dua jenis bangunan yang dapat didirikan: rumah dan hotel. Rumah dapat dibangun hingga
maksimal 4 buah per petak. Setelah seluruh petak dalam satu _color group_ memiliki 4 rumah
masing-masing, pemain dapat meng- _upgrade_ ke hotel. Hanya boleh ada 1 hotel per petak dan hotel
merupakan tingkat bangunan tertinggi yang dapat dicapai. Petak yang sudah berstatus hotel **tidak
dapat** dibangun lebih lanjut.

Pembangunan rumah harus dilakukan secara merata di seluruh petak dalam satu _color group_.
Artinya, selisih jumlah rumah antar petak dalam satu _color group_ tidak boleh melebihi 1 pada setiap
saat. Pemain tidak dapat menambah rumah ke-2 di suatu petak sebelum seluruh petak lain dalam
_color group_ yang sama memiliki setidaknya 1 rumah. Petak yang tidak memenuhi syarat
pemerataan ini tidak akan ditampilkan sebagai pilihan pada perintah **BANGUN**.

### Lelang Tanah.................................................................................................................................................................

Ketika seorang pemain berhenti pada petak properti _street_ , ia dapat memilih untuk TIDAK
membeli properti tersebut (atau tidak mampu membeli). Ketika hal ini terjadi, petak properti
tersebut akan secara otomatis dilelang. Ketika seorang pemain bangkrut ke Bank dan permainan
masih terus berlanjut, SEMUA petak properti yang dimiliki pemain yang bangkrut tersebut akan
otomatis dilelang satu per satu. Urutan properti yang dilelang dibebaskan.

#### Mekanisme Lelang.............................................................................................................................................

Berikut merupakan langkah-langkah terjadinya lelang:

1. Lelang dimulai dari pemain yang memiliki giliran setelah pemain yang memicu lelang, lalu
    berlanjut sesuai urutan giliran permainan.
2. Pada gilirannya, setiap pemain hanya dapat memilih satu dari dua aksi:
    a. _Pass_ : Tidak mengajukan penawaran dan melewatkan giliran.
    b. _Bid_ : Mengajukan penawaran harga untuk membeli properti.
3. Penawaran pertama ( _bid_ awal) minimal adalah M0. Setiap _bid_ berikutnya harus lebih tinggi
    dari bid sebelumnya.
4. Lelang berlanjut hingga terjadi _pass_ berturut-turut sebanyak (jumlah pemain - 1). Jika
    lelang terjadi karena kondisi bangkrut, jumlah pemain tidak menghitung pemain yang
    bangkrut tersebut. Jika kondisi tersebut tercapai, maka:
       a. Pemain yang terakhir melakukan _bid_ menjadi pemenang lelang.


```
b. Pemenang membayar sesuai nilai bid terakhir.
c. Properti diberikan kepada pemenang.
```
5. Minimal terdapat satu pemain yang melakukan _bid_. Jika terjadi kondisi (4) tanpa ada
    pemain yang melakukan _bid_ , maka pemain terakhir yang tidak melakukan _pass_ harus
    melakukan _bid_. Lelang berlanjut seperti biasa.
6. Pemain tidak dapat melakukan _bid_ melebihi uang yang dimiliki.
7. Setelah lelang selesai, permainan kembali ke urutan giliran normal (bukan berdasarkan
    urutan lelang).
8. Giliran lelang bukan bagian dari giliran permainan biasa. Giliran lelang tidak mempengaruhi
    durasi efek apa pun atau perhitungan giliran untuk batas maksimum jumlah giliran.

### Pajak................................................................................................................................................................................

Terdapat dua petak pajak dalam permainan yang akan dikenakan kepada pemain apabila token
mereka mendarat di atasnya. Kedua petak ini tidak dapat dimiliki oleh siapapun dan pajaknya
langsung dibayarkan ke Bank.

Petak pertama adalah **Pajak Penghasilan (PPH)**. Ketika pemain mendarat di petak ini, pemain
dihadapkan pada dua pilihan: membayar sejumlah _flat_ , atau membayar persentase dari total
kekayaan mereka. **Pemain harus memutuskan pilihan mana yang akan diambil sebelum
menghitung total kekayaannya.** Total kekayaan dihitung dari seluruh uang tunai yang dimiliki,
ditambah harga beli seluruh properti yang dimiliki baik yang sedang digadaikan maupun tidak,
ditambah harga beli seluruh bangunan yang telah didirikan. Jika pemain memilih opsi _flat_ namun
uangnya tidak mencukupi, pemain langsung memasuki kondisi kebangkrutan tanpa ditawarkan
opsi kedua. Persentase pembayaran serta jumlah _flat_ yang perlu dibayarkan didefinisikan di file
konfigurasi.

Petak kedua adalah **Pajak Barang Mewah (PBM)**. Ketika pemain mendarat di petak ini, pemain
langsung dikenakan pajak sebesar nilai yang didefinisikan di file konfigurasi tanpa pilihan lain.

Apabila pemain tidak mampu membayar pajak (baik PPH maupun PBM), pemain memasuki kondisi
kebangkrutan dengan Bank sebagai kreditor. Alur kebangkrutan ditangani oleh bagian
Kebangkrutan dan Pengambilalihan Aset.

### Festival...........................................................................................................................................................................

Petak festival merupakan petak aksi yang dapat melipatgandakan harga sewa dari properti yang
dimiliki. Ketika seorang pemain berhenti di petak festival, pemain tersebut dapat memilih salah
satu dari properti yang ia miliki. Harga sewa dari properti yang dipilih akan naik dua kali lipat
selama tiga giliran. Jika pemain berhenti kembali di petak festival dan memilih properti yang sama,
durasi dikembalikan menjadi tiga giliran (jika durasi sudah berkurang) dan harga sewa naik dua kali


lipat lagi. Hal ini dapat berulang hingga tiga kali (maksimum harga sewa naik delapan kali lipat),
selebihnya hanya mengembalikan durasi.

Berikut contoh skenario yang mungkin terjadi:

1. Pemain A berhenti pada petak festival lalu memilih properti Z (harga sewa awal 5.000).
    Harga sewa properti Z menjadi 10.000 selama tiga giliran pemain A.
2. Giliran pemain A lagi, durasi festival pada properti Z berkurang satu. Pemain A berhenti di
    petak festival lagi dan memilih properti Y (harga sewa dari 2.000 menjadi 4.000) selama
    tiga giliran. Perhitungan durasi terpisah untuk setiap properti (Z tersisa dua giliran dan Y
    tiga giliran).
3. Giliran pemain A lagi, durasi Z = 1, Y = 2. Pemain A berhenti di petak festival lagi dan
    memilih properti Z. Harga sewa properti Z menjadi 20.000 dan durasi kembali menjadi tiga
    giliran.
4. Skenario (3) terulang kembali, durasi Z = 2, Y = 1. Harga sewa properti Z menjadi 40.
    dan durasi kembali menjadi tiga giliran.
5. Skenario (3) terulang kembali, durasi Z = 2, Y = 0. Harga sewa properti Y kembali seperti
    semula. Harga sewa properti Z tetap 40.000 (max kenaikan harga sewa sebanyak tiga kali).
    Durasi kembali menjadi tiga giliran.

### Kartu Kesempatan dan Dana Umum.................................................................................................................

Dalam Permainan Nimonspoli, terdapat tumpukan Kartu Kesempatan dan Dana Umum. Papan
permainan juga memiliki beberapa petak khusus yang mewakili kedua jenis kartu tersebut. Apabila
seorang pemain mendarat di salah satu petak ini, ia wajib mengambil kartu teratas dari tumpukan
yang sesuai dan segera menjalankan instruksi yang tertera di dalamnya. Setelah digunakan kartu
akan kembali di _reshuffle_ dalam tumpukan.

#### Kartu Kesempatan.............................................................................................................................................

Berikut adalah aturan Kartu Kesempatan yang ada dalam Permainan Nimonspoli:

1. "Pergi ke stasiun terdekat."
2. "Mundur 3 petak."
3. "Masuk Penjara."

#### Kartu Dana Umum.............................................................................................................................................

Berikut adalah aturan Kartu Dana Umum yang ada dalam Permainan Nimonspoli:

1. "Ini adalah hari ulang tahun Anda. Dapatkan M100 dari setiap pemain."
2. "Biaya dokter. Bayar M700."
3. "Anda mau nyaleg. Bayar M200 kepada setiap pemain."


### Kartu Kemampuan Spesial.....................................................................................................................................

Selain Kartu Kesempatan dan Dana Umum terdapat pula tumpukan Kartu Kemampuan Spesial.
Kartu Kemampuan Spesial merupakan kartu tangan yang memberikan keuntungan taktis bagi
pemain.

#### Aturan Kartu Kemampuan Spesial..............................................................................................................

Berikut adalah aturan Kartu Kemampuan Spesial:

1. Penggunaan kartu dibatasi maksimal 1 kali dalam 1 giliran.
2. Pada awal giliran, semua pemain akan mendapatkan masing-masing 1 kartu acak.
3. Setiap pemain hanya boleh memiliki maksimal 3 kartu di tangannya.
4. Jika seorang pemain mendapatkan kartu keempat, ia diwajibkan untuk membuang 1 kartu
    yang dimilikinya.
5. Kartu ini bersifat sekali pakai. Setelah dipakai, kartu akan dibuang dan diletakkan di
    tempat pembuangan.
6. Apabila kartu di dalam deck habis, seluruh kartu di tempat pembuangan akan dikocok
    ulang untuk membuat deck baru.
7. Kartu ini hanya bisa digunakan sebelum melempar dadu.

#### Jenis Kartu Kemampuan Spesial..................................................................................................................

Terdapat beberapa jenis Kartu Kemampuan Spesial dalam Permainan Nimonspoli:

1. **MoveCard** (4 lembar): Memungkinkan pemain untuk bergerak maju sekian petak. Nominal
    langkah pergerakan ditentukan secara acak pada saat pemain pertama kali mendapatkan
    kartu ini.
2. **DiscountCard** (3 lembar): Memberikan diskon dengan persentase acak kepada pemain
    saat kartu didapatkan. Masa berlaku DiscountCard adalah 1 giliran.
3. **ShieldCard** (2 lembar): Melindungi pemain dari tagihan sewa maupun sanksi apapun yang
    merugikan selama 1 giliran.
4. **TeleportCard** (2 lembar): Memberikan pemain kebebasan untuk berpindah ke petak
    manapun di atas papan permainan.
5. **LassoCard** (2 lembar): Menarik satu pemain lawan yang berada di depan posisi pemain saat
    ini ke petak tempat pemain tersebut berada.
6. **DemolitionCard** (2 lembar): Menghancurkan satu properti milik pemain lawan.

### Bangkrut........................................................................................................................................................................

Kondisi bangkrut ( _bankruptcy_ ) terjadi ketika seorang pemain tidak mampu memenuhi kewajiban
pembayaran, baik berupa sewa, pajak, maupun efek dari kartu tertentu. Ketika pemain tidak
memiliki cukup uang tunai ( _cash_ ) untuk membayar kewajibannya, sistem akan menghitung jumlah
maksimum dana yang masih dapat diperoleh melalui proses likuidasi aset. Jika kewajiban masih
dapat dipenuhi melalui likuidasi, pemain **wajib melakukan likuidasi tersebut**. Namun, jika setelah


seluruh kemungkinan likuidasi dilakukan pemain tetap tidak mampu membayar, maka pemain
dinyatakan **bangkrut**.

#### Mekanisme Likuidasi Aset...............................................................................................

Pemain dapat melakukan likuidasi aset melalui beberapa cara berikut:

1. **Menjual Properti ke Bank**
    ○ Properti dapat dijual kembali ke bank dengan harga sesuai nilai beli yang tertera
       pada akta kepemilikan.
    ○ Jika properti berupa _street_ dan memiliki bangunan (rumah atau hotel), maka nilai
       jual properti akan ditambah dengan **setengah dari total harga bangunan** yang ada
       di atasnya.
2. **Menggadaikan (Mortgage) Properti**
    ○ Properti dapat digadaikan ke bank sesuai dengan ketentuan pegadaian.
    ○ Pemain akan menerima sejumlah uang berdasarkan nilai gadai properti tersebut.
    ○ Properti yang sedang digadaikan tidak dapat menghasilkan pendapatan (sewa).

Pemain bebas memilih kombinasi likuidasi mana yang ingin dilakukan, selama total dana yang
dapat diperoleh masih memungkinkan pelunasan.

### Pengambilalihan Asset.............................................................................................................................................

Pengambilalihan Aset terjadi ketika pemain dinyatakan bangkrut dan dibedakan berdasarkan
siapa yang menjadi kreditor:
● **Bangkrut ke pemain lain** : Seluruh aset (uang sisa dan properti beserta bangunan di
atasnya) diserahkan langsung ke pemain kreditor. Properti yang sedang digadaikan
diserahkan dalam kondisi tergadai sehingga kreditor menanggung status gadai tersebut.
● **Bangkrut ke Bank** (akibat pajak atau kartu): Seluruh uang sisa diserahkan ke Bank dan
hilang dari peredaran. Seluruh properti dikembalikan ke status BANK dan dilelang satu per
satu. Bangunan dihancurkan dan stok dikembalikan ke Bank.

Setelah bangkrut, pemain keluar dari permainan. Giliran lelang aset tidak dihitung sebagai giliran
permainan biasa dan tidak mempengaruhi durasi efek apapun.

**Kondisi kelanjutan permainan** setelah kebangkrutan:
● Jika masih ada ≥ 2 pemain aktif → permainan berlanjut normal.
● Jika hanya tersisa 1 pemain → pemain tersebut menang.

### Kondisi Game Selesai...............................................................................................................................................

Permainan Nimonspoli berakhir ketika mencapai salah satu dari dua kondisi: **_Bankruptcy_** atau **_Max
Turn_**.


#### MAX TURN...........................................................................................................................................................

Permainan Nimonspoli berakhir ketika semua pemain telah mencapai batas maksimum giliran.
Jumlah batas maksimum giliran didefinisikan pada **_file config_**. Pemenang adalah pemain dengan
uang terbanyak. Jika seri, pemain yang memiliki jumlah petak properti terbanyak yang menjadi
pemenang. Jika seri, pemain yang memiliki jumlah kartu terbanyak yang menjadi pemenang. Jika
seri, semua pemain yang masih seri menjadi pemenang. Jika kondisi _bankruptcy_ terjadi sebelum
mencapai batas maksimum giliran, ikuti aturan _bankruptcy_.

#### BANKRUPTCY....................................................................................................................................................

Jika batas maksimum giliran bukan merupakan angka yang valid (<1) pada **_file config_** , Nimonspoli
berakhir secara _bankruptcy_. Permainan Nimonspoli terus berlanjut tanpa batas turn hingga hanya
tersisa satu pemain yang tidak bangkrut. Pemain tersebut dinyatakan sebagai pemenang.

### Save/Load......................................................................................................................................................................

Sistem Save/Load memungkinkan seluruh state permainan disimpan ke file eksternal dengan
ekstensi .txt dan dimuat kembali di sesi berikutnya. Penyimpanan data hanya dapat dilakukan di
awal giliran pemain, saat belum ada aksi apapun yang dijalankan oleh pemain tersebut (termasuk
melemparkan dadu), sedangkan muat data hanya dapat dilakukan saat program berjalan pertama
kali (sebelum permainan dimulai).

**Format File Save** menggunakan struktur teks terstruktur sebagai berikut:

```
<TURN_SAAT_INI> <MAX_TURN>
<JUMLAH_PEMAIN>
<STATE_PEMAIN_1>
<STATE_PEMAIN_2>
...
<STATE_PEMAIN_N>
<URUTAN_GILIRAN_1> <URUTAN_GILIRAN_2> ... <URUTAN_GILIRAN_N>
<GILIRAN_AKTIF_SAAT_INI>
<STATE_PROPERTI>
<STATE_DECK>
<STATE_LOG>
```
**Format State Pemain**

```
<USERNAME> <UANG> <POSISI_PETAK> <STATUS>
<JUMLAH_KARTU_TANGAN>
<JENIS_KARTU_1> <NILAI_KARTU_1> <SISA_DURASI_1>
<JENIS_KARTU_2> <NILAI_KARTU_2> <SISA_DURASI_2>
```
**Keterangan:
● <USERNAME>** adalah username pemain
**● <UANG>** adalah jumlah uang pemain
● **<POSISI_PETAK>** adalah kode petak tempat pemain berada


```
● <STATUS> berisi status dari pemain yang bernilai ACTIVE, BANKRUPT, atau JAILED (jika
terjebak di penjara)
● <JENIS_KARTU> adalah jenis kartu kemampuan (contoh: MoveCard , ShieldCard )
● <NILAI_KARTU> adalah parameter nilai kartu tersebut (contoh: jumlah langkah untuk
MoveCard, persentase diskon untuk DiscountCard).Untuk card yang tidak memiliki nilai
tertentu, kosongkan nilai ini
● <SISA_DURASI> adalah sisa durasi berlakunya kartu tersebut, hanya untuk kartu jenis
DiscountCard. Untuk kartu jenis lain, kosongkan nilai ini
```
**Format State Properti**

```
<JUMLAH_PROPERTI>
<KODE_PETAK> <JENIS> <PEMILIK> <STATUS> <FMULT> <FDUR> <N_BANGUNAN>
...
```
Keterangan
● **<KODE_PETAK>** adalah kode petak dari properti
● **<JENIS>** adalah jenis properti yang bernilai **street** , **railroad** , atau **utility
● <PEMILIK>** berisi username pemilik atau **BANK** jika belum dimiliki
● **<STATUS>** adalah status dari properti yang bernilai **BANK** , **OWNED** , atau **MORTGAGED**
● **<FMULT>** adalah harga bangunan akibat festival yang bernilai 1, 2, 4, atau 8 (1 = tidak
aktif)
● **<FDUR>** adalah durasi dari festival yang bernilai 0–3 (0 = tidak aktif)
● **<N_BANGUNAN>** bernilai 0–4 untuk rumah, atau H untuk hotel. Untuk properti selain _street_ ,
isi dengan 0.

**Format State Deck**

```
<JUMLAH_KARTU_DECK_KEMAMPUAN>
<JENIS_KARTU_1>
...
```
**Format State Log**

```
<JUMLAH_ENTRI_LOG>
<TURN> <USERNAME> <JENIS_AKSI> <DETAIL>
...
```
Berikut adalah contoh isi file yang berisi state yang disimpan.

```
15 30
4
Uname1 1500 JKT ACTIVE
2
MoveCard 5
ShieldCard
Uname2 800 BDG ACTIVE
```

##### 0

```
Uname3 2200 PKR ACTIVE
1
DiscountCard 30 1
Uname4 0 PJR BANKRUPT
0
Uname2 Uname4 Uname1 Uname3
Uname2
22
JKT street Uname1 OWNED 2 2 1
BDG street Uname2 MORTGAGED 0 1 0
KAI railroad Uname1 OWNED 1 0 0
SBY street BANK BANK 0 1 0
PLN utility Uname3 OWNED 1 0 0
```
```
12
MoveCard
DiscountCard
TeleportCard
```
##### 10

```
1 Uname1 BELI Beli Jakarta (JKT) seharga M400
1 Uname2 DADU Lempar: 4+5=9 mendarat di Surabaya (SBY)
```
### Transaction Logger....................................................................................................................................................

Transaction Logger mencatat setiap kejadian signifikan dalam permainan secara otomatis ke dalam
log terstruktur. Logger bekerja di latar belakang dan tidak memerlukan perintah khusus untuk
aktif.

Setiap entri log memuat informasi berikut: nomor turn, username pemain yang relevan, jenis aksi,
dan detail aksi (termasuk nilai uang jika ada). Jenis aksi yang dicatat meliputi:

```
● Lemparan dadu dan pergerakan bidak
● Pembelian properti (termasuk perolehan Railroad/Utility otomatis)
● Pembayaran sewa dan pajak
● Pembangunan dan penjualan bangunan
● Gadai dan tebus properti
● Penggunaan kartu kemampuan spesial
● Pengambilan kartu Kesempatan dan Dana Umum
● Setiap bid dan hasil akhir lelang
● Aktivasi dan penguatan efek festival
● Kebangkrutan dan pengambilalihan aset
● Save dan load permainan
```

Logger menyimpan _full log_ selama sesi berlangsung di memori. Saat game di-save, _full log_ ikut
tersimpan ke dalam file.

## Operasi dan Perintah..............................................................................................................................................

### 1. CETAK_PAPAN.................................................................................................................

```
Perintah untuk menampilkan papan permainan. Ketentuan visualisasi papan permainan
adalah sebagai berikut:
● Gunakan Colored Print untuk petak lahan sesuai dengan kategori/warnanya.
● Tampilkan posisi bidak semua pemain dengan jelas di semua kemungkinan skenario.
● Tampilkan semua kepemilikan dan status lahan dengan jelas.
● Tampilkan legenda papan permainan serta indikator giliran.
```
##### > CETAK_PAPAN

+----------|[DF] BBP +----------+|[MR] MKS |----------[DF] KSP +----------+----------+|[MR] BLP |[MR] MND |----------[DF] TUG +----------+----------|[KN] PLB |[KN] PKB +----------|[AB] PAM +----------+|[KN] MED |----------+[DF] PPJ | (^)
|+---------- (1) |P1+----------+ ^ | (^) ----------|P2+----------+----------+ ^^ |P3 ^^^ |P1---------- |P2+----------+---------- ^ |P2 ^ |P4+---------- |P2+----------+ ^^ | (^) ----------+| (^)
|[OR]|P3 * SBY || |[HJ]|P4 ^ BDG || (^)
+----------+|[OR] SMG | (^) ================================== +----------+|[HJ] DEN | (^)
|P3+----------+ ^^ | ||================================== NIMONSPOLI || |P4+----------+ ^ | (^)
|[DF]| DNU || (^) TURN 15 / 50 |[DF]| FES || (^)
+----------+|[OR] MAL | +----------+|[HJ] MTR | (^)
|P3+----------+ ^ | ----------------------------------LEGENDA KEPEMILIKAN & STATUS |P4+----------+ ^^ | (^)
|[DF]|P1 STB || P1-P4^ :: PropertiRumah Level milik 1 Pemain 1-4 |[DF]|P2 GUB || (^)
+----------+|[PK] YOG | ^^^^^ :: RumahRumah LevelLevel 23 +----------+|[DF] KSP | (^)
|P4+----------+ ^^^ | *(1)-(4): : Hotel Bidak(Maksimal) (IN=Tahanan, (^) V=Mampir) |+----------+ | (^)
|[PK]|P4 ^^ SOL || ----------------------------------KODE WARNA: |[BT]|P1 ^ JKT(4)| | (^)
+----------+|[AB] PLN | [CK]=Coklat[BM]=Biru Muda [MR]=Merah[KN]=Kuning +----------+|[DF] PBM | (^)
|P2+----------+ | [PK]=Pink[OR]=Orange [HJ]=Hijau[BT]=Biru Tua |+----------+ | (^)
|[PK]|P4 ^ MGL || [DF]=Aksi [AB]=Utilitas |[BT]|P1 * IKN || (^)
+----------|[DF] PEN +----------+----------+|[BM] BKS |[BM] DPK |----------[DF] FES +----------+|[BM] BGR |----------+----------[DF] GBR |[DF] PPH +----------+|[CK] TSK |----------[DF] DNU +----------+|[CK] GRT |----------+[DF] GO | (^)
|+---------- IN:2 V:3 |P2+----------+----------+ ^ |P2 ^^ | (^) ----------|P2+----------+ ^^^ |P3----------+---------- | |P1+----------+ ^^ | (^) ----------|P1+----------+ ^ | (^) ----------+| (^)

### 2. LEMPAR_DADU................................................................................................................

```
Perintah untuk melempar dadu secara random.
```
##### > LEMPAR_DADU

```
Mengocok dadu...
```

```
Hasil: 6 + 1 = 7
Memajukan Bidak Pemain1 sebanyak 7 petakk...
Bidak mendarat di: Bandung.
```
```
// Jika bidak mendarat di Petak Lahan yang belum dimiliki
siapa-siapa, lanjut ke skenario BELI
```
```
// Jika bidak mendarat di Petak Lahan yang sudah dimiliki
seseorang, lanjut ke skenario BAYAR_SEWA
```
```
// Jika bidak mendarat di Petak Pajak, lanjut ke skenario
BAYAR_PAJAK
```
### 3. ATUR_DADU X Y..............................................................................................................

```
Perintah untuk mengatur hasil lemparan dadu secara manual.
```
##### > ATUR_DADU 2 5

```
Dadu diatur secara manual.
Hasil: 2 + 5 = 7
Memajukan Bidak Pemain1 sebanyak 7 petakk...
Bidak mendarat di: Bandung.
```
### 4. CETAK_AKTA..........................................................................................................................................................

```
Perintah untuk menampilkan informasi lengkap Akta Kepemilikan dari sebuah properti.
Dapat dipanggil kapan saja selama giliran pemain.
```
##### > CETAK_AKTA

```
Masukkan kode petak: JKT
```
```
+================================+
| AKTA KEPEMILIKAN |
| [BIRU TUA] JAKARTA (JKT) |
+================================+
| Harga Beli : M400 |
| Nilai Gadai : M200 |
+--------------------------------+
| Sewa (unimproved) : M50 |
| Sewa (1 rumah) : M200 |
| Sewa (2 rumah) : M600 |
| Sewa (3 rumah) : M1.400 |
```

```
| Sewa (4 rumah) : M1.700 |
| Sewa (hotel) : M2.000 |
+--------------------------------+
| Harga Rumah : M200 |
| Harga Hotel : M200 |
+================================+
| Status : OWNED (Pemain A) |
+================================+
```
```
// Jika kode tidak ditemukan:
Petak "XYZ" tidak ditemukan atau bukan properti.
```
### 5. CETAK_PROPERTI...............................................................................................................................................

```
Perintah untuk menampilkan seluruh properti yang dimiliki oleh pemain yang sedang
bermain beserta statusnya.
```
##### > CETAK_PROPERTI

```
=== Properti Milik: Pemain A ===
```
```
[BIRU TUA]
```
- Jakarta (JKT) 1 rumah M400 OWNED
- Ibu Kota Nusantara (IKN) Hotel M400 OWNED

```
[HIJAU]
```
- Bandung (BDG) M300 MORTGAGED [M]
- Denpasar (DEN) M300 OWNED
- Mataram (MTR) M320 OWNED

```
[STASIUN]
```
- Stasiun Gambir (GBR) M200 OWNED
- Stasiun Tugu (TUG) M200 OWNED

```
Total kekayaan properti: M3.720
```
```
// Jika tidak punya properti:
Kamu belum memiliki properti apapun.
```
### 6. BELI (OTOMATIS).................................................................................................................................................

```
Perintah ini dipicu secara otomatis ketika pemain mendarat di petak properti berstatus
BANK. Untuk Railroad dan Utility, kepemilikan langsung berpindah tanpa prompt
pembelian.
```

```
// Kasus Street:
Kamu mendarat di Jakarta (JKT)!
+================================+
| [BIRU TUA] JAKARTA (JKT) |
| Harga Beli : M400 |
| Sewa dasar : M50 |
| ... |
+================================+
Uang kamu saat ini: M1.500
Apakah kamu ingin membeli properti ini seharga M400? (y/n): y
Jakarta kini menjadi milikmu!
Uang tersisa: M1.100
```
```
---
// Jika tidak mau atau uang tidak cukup:
Properti ini akan masuk ke sistem lelang...
```
```
---
// Kasus Railroad:
Kamu mendarat di Stasiun Gambir (GBR)!
Belum ada yang menginjaknya duluan, stasiun ini kini menjadi
milikmu!
```
```
---
// Kasus Utility:
Kamu mendarat di PLN!
Belum ada yang menginjaknya duluan, PLN kini menjadi milikmu!
```
### 7. BAYAR_SEWA (OTOMATIS).............................................................................................................................

```
Perintah ini dipicu secara otomatis ketika pemain mendarat di properti berstatus OWNED
milik pemain lain.
```
```
Kamu mendarat di Jakarta (JKT), milik Pemain B!
```
```
Kondisi : 2 rumah
Sewa : M600
```
```
Uang kamu : M1.200 -> M600
Uang Pemain B : M800 -> M1.400
```
```
---
```
```
// Jika properti digadaikan:
Kamu mendarat di Jakarta (JKT), milik Pemain B.
Properti ini sedang digadaikan [M]. Tidak ada sewa yang
dikenakan.
```

##### ---

```
// Jika tidak mampu bayar:
Kamu tidak mampu membayar sewa penuh! (M600)
Uang kamu saat ini: M200
// Alur dilanjutkan ke Kebangkrutan
```
### 8. BAYAR_PAJAK (OTOMATIS)............................................................................................................................

```
Perintah ini dipicu secara otomatis ketika pemain mendarat di petak PPH atau PBM.
```
```
// Kasus PPH:
Kamu mendarat di Pajak Penghasilan (PPH)!
Pilih opsi pembayaran pajak:
```
1. Bayar flat M150
2. Bayar 10% dari total kekayaan
(Pilih sebelum menghitung kekayaan!)
Pilihan (1/2): 2

```
Total kekayaan kamu:
```
- Uang tunai : M1.500
- Harga beli properti : M1.400 (termasuk yang digadaikan)
- Harga beli bangunan : M300
Total : M3.200
Pajak 10% : M320
Uang kamu: M1.500 -> M1.180

```
---
// Jika pilih flat tapi uang tidak cukup:
Kamu mendarat di Pajak Penghasilan (PPH)!
Pilih opsi pembayaran pajak:
```
1. Bayar flat M150
2. Bayar 10% dari total kekayaan
(Pilih sebelum menghitung kekayaan!)
Pilihan (1/2): 1

```
Kamu tidak mampu membayar pajak flat M150!
Uang kamu saat ini: M80
// Alur dilanjutkan ke Kebangkrutan
```
```
---
// Kasus PBM:
Kamu mendarat di Pajak Barang Mewah (PBM)!
Pajak sebesar M150 langsung dipotong.
Uang kamu: M1.500 -> M1.350
```

##### ---

```
// Jika tidak mampu bayar PBM:
Kamu mendarat di Pajak Barang Mewah (PBM)!
Pajak sebesar M150 langsung dipotong.
Kamu tidak mampu membayar pajak!
Uang kamu saat ini: M80
// Alur dilanjutkan ke Kebangkrutan
```
### 9. GADAI........................................................................................................................................................................

```
Perintah untuk menggadaikan properti milik pemain ke Bank. Dapat dilakukan kapan saja
selama giliran pemain.
```
```
> GADAI
=== Properti yang Dapat Digadaikan ===
```
1. Jakarta (JKT) [BIRU TUA] Nilai Gadai: M200
2. Stasiun Gambir (GBR) [STASIUN] Nilai Gadai: M100

```
Pilih nomor properti (0 untuk batal): 1
Jakarta berhasil digadaikan.
Kamu menerima M200 dari Bank.
Uang kamu sekarang: M700
Catatan: Sewa tidak dapat dipungut dari properti yang
digadaikan.
```
```
---
// Jika masih ada bangunan di color group:
Jakarta tidak dapat digadaikan!
Masih terdapat bangunan di color group [BIRU TUA].
Bangunan harus dijual terlebih dahulu.
```
```
Daftar bangunan di color group [BIRU TUA]:
```
1. Jakarta (JKT) - 2 rumah -> Nilai jual bangunan:
M200
2. Ibu Kota Nusantara (IKN) - 1 rumah -> Nilai jual bangunan:
M100

```
Jual semua bangunan color group [BIRU TUA]? (y/n): y
Bangunan Jakarta terjual. Kamu menerima M200.
Bangunan Ibu Kota Nusantara terjual. Kamu menerima M100.
Uang kamu sekarang: M800
```
```
Lanjut menggadaikan Jakarta? (y/n): y
Jakarta berhasil digadaikan.
Kamu menerima M200 dari Bank.
Uang kamu sekarang: M1.000
Catatan: Sewa tidak dapat dipungut dari properti yang
```

```
digadaikan.
```
```
---
// Jika tidak ada yang bisa digadaikan:
Tidak ada properti yang dapat digadaikan saat ini.
```
### 10. TEBUS.....................................................................................................................................................................

```
Perintah untuk menebus properti yang sedang digadaikan. Dapat dilakukan kapan saja
selama giliran pemain.
```
##### > TEBUS

```
=== Properti yang Sedang Digadaikan ===
```
1. Jakarta (JKT) [BIRU TUA] [M] Harga Tebus: M400
2. Bandung (BDG) [HIJAU] [M] Harga Tebus: M300

```
Uang kamu saat ini: M800
Pilih nomor properti (0 untuk batal): 1
```
```
Jakarta berhasil ditebus!
Kamu membayar M400 ke Bank.
Uang kamu sekarang: M400
```
```
---
```
```
// Jika uang tidak cukup:
Uang kamu tidak cukup untuk menebus Jakarta.
Harga tebus: M400 | Uang kamu: M150
```
```
---
```
```
// Jika tidak ada yang digadaikan:
Tidak ada properti yang sedang digadaikan.
```
### 11. BANGUN...............................................................................................................................................................

```
Perintah untuk membangun rumah atau mengupgrade ke hotel. Dapat dilakukan kapan
saja selama giliran pemain.
```
##### > BANGUN

```
=== Color Group yang Memenuhi Syarat ===
```
1. [BIRU TUA]


- Jakarta (JKT) : 1 rumah (Harga rumah: M200)
- Ibu Kota Nusantara (IKN) : 1 rumah (Harga rumah: M200)
2. [HIJAU]
- Bandung (BDG) : 0 rumah (Harga rumah: M150)
- Denpasar (DEN) : 0 rumah (Harga rumah: M150)
- Mataram (MTR) : 0 rumah (Harga rumah: M150)

Uang kamu saat ini : M1.500
Pilih nomor color group (0 untuk batal): 1

Color group [BIRU TUA]:
// Hanya petak yang memenuhi syarat pemerataan yang
ditampilkan

- Jakarta (JKT) : 1 rumah <- dapat dibangun
- Ibu Kota Nusantara (IKN) : 1 rumah <- dapat dibangun

Pilih petak (0 untuk batal): 1 // Jakarta
Kamu membangun 1 rumah di Jakarta. Biaya: M200
Uang tersisa: M1.300

- Jakarta (JKT) : 2 rumah
- Ibu Kota Nusantara (IKN) : 1 rumah

---
// Upgrade ke hotel (saat semua petak sudah punya 4 rumah):
Color group [BIRU TUA]:

- Jakarta (JKT) : 4 rumah <- siap upgrade ke hotel
- Ibu Kota Nusantara (IKN) : 4 rumah <- siap upgrade ke hotel

Seluruh color group [BIRU TUA] sudah memiliki 4 rumah. Siap
di-upgrade ke hotel!
Pilih petak (0 untuk batal): 1 // Jakarta
Upgrade ke hotel? Biaya: M200 (y/n): y
Jakarta di-upgrade ke Hotel!
Uang tersisa: M1.100

---
// Jika petak sudah berstatus hotel:
Color group [BIRU TUA]:

- Jakarta (JKT) : Hotel <- sudah maksimal, tidak
dapat dibangun
- Ibu Kota Nusantara (IKN) : 4 rumah <- siap upgrade ke hotel

Pilih petak (0 untuk batal): 2 // Ibu Kota Nusantara
Upgrade ke hotel? Biaya: M200 (y/n): y
Ibu Kota Nusantara di-upgrade ke Hotel!
Uang tersisa: M900

---
// Jika syarat monopoli belum terpenuhi:
Tidak ada color group yang memenuhi syarat untuk dibangun.
Kamu harus memiliki seluruh petak dalam satu color group


### 12. LELANG (OTOMATIS).....................................................................................................

```
Perintah ini dipicu otomatis ketika salah satu dari kondisi berikut terpenuhi:
● Pemain menolak membeli properti
● Pemain tidak mampu membeli
● Pemain bangkrut ke Bank (semua properti dilelang)
```
```
terlebih dahulu.
```
```
Properti Jakarta (JKT) akan dilelang!
```
```
Urutan lelang dimulai dari pemain setelah Pemain A.
```
```
Giliran: Pemain B
Aksi (PASS / BID <jumlah>)
> BID 100
Penawaran tertinggi: M100 (Pemain B)
```
```
Giliran: Pemain C
Aksi (PASS / BID <jumlah>)
> BID 150
Penawaran tertinggi: M150 (Pemain C)
Giliran: Pemain D
Aksi (PASS / BID <jumlah>)
> PASS
```
```
Giliran: Pemain A
Aksi (PASS / BID <jumlah>):
> PASS
```
```
Giliran: Pemain B
Aksi (PASS / BID <jumlah>):
> PASS
```
```
Lelang selesai!
Pemenang: Pemain C
Harga akhir: M150
```
```
Properti Jakarta (JKT) kini dimiliki Pemain C.
```

### 13. FESTIVAL (OTOMATIS)..................................................................................................

```
Perintah ini dipicu saat pemain mendarat di petak festival.
```
(^) Kamu mendarat di petak Festival!
Daftar properti milikmu:

- JKT (Jakarta)
- BDG (Bandung)
- DEN (Denpasar)

```
Masukkan kode properti: JKT
```
```
Efek festival aktif!
```
```
Sewa awal: M5000
Sewa sekarang: M10000
Durasi: 3 giliran
```
```
// SKENARIO MEMILIH FESTIVAL YANG SAMA
Masukkan kode properti: JKT
```
```
Efek diperkuat!
```
```
Sewa sebelumnya: M10000
Sewa sekarang: M20000
Durasi di-reset menjadi: 3 giliran
```
```
// SKENARIO MEMILIH FESTIVAL YANG SAMA NAMUN MULTIPLIER SEWA
MAKSIMAL
Masukkan kode properti: JKT
```
```
Efek sudah maksimum (harga sewa sudah digandakan tiga kali)
Durasi di-reset menjadi: 3 giliran
```
```
// SKENARIO ERROR (TAMPILAN DIBEBASKAN, DI BAWAH HANYA CONTOH)
```
```
Masukkan kode properti: XYZ
-> Kode properti tidak valid!
```
```
Masukkan kode properti: MND
-> Properti bukan milikmu!
// Informasi mengenai efek festival ditampilkan juga melalui
output CETAK_AKTA, CETAK_PROPERTI, dan BAYAR_SEWA. Cukup
tampilkan keterangan efek festival aktif, sisa durasi, dan
harga sewa terbaru
```

### 14. BANGKRUT DAN AMBIL ALIH ASET (OTOMATIS).......................................................

```
Perintah ini dipicu otomatis ketika pemain tidak memiliki cukup uang tunai untuk
membayar kewajiban. Sistem terlebih dahulu menghitung potensi dana dari likuidasi
seluruh aset. Jika likuidasi dapat menutup kewajiban, pemain diarahkan ke panel likuidasi
dan wajib melikuidasi hingga kewajiban terpenuhi. Jika tidak, pemain langsung dinyatakan
bangkrut.
```
```
Skenario 1: Likuidasi dapat menutup kewajiban
```
```
Skenario 2: Likuidasi tidak dapat menutup kewajiban pembayaran ke pemain lain
Kamu tidak dapat membayar sewa M1200 kepada Uname2!
```
```
Kamu tidak dapat membayar sewa M600 kepada Uname2!
```
```
Uang kamu : M200
Total kewajiban : M600
Kekurangan : M400
```
```
Estimasi dana maksimum dari likuidasi:
Jual Jakarta (JKT) [MERAH] → M400
Gadai Kupang (KPG) [UNGU] → M100
Total potensi → M500
```
```
Dana likuidasi dapat menutup kewajiban.
Kamu wajib melikuidasi aset untuk membayar.
```
```
=== Panel Likuidasi ===
Uang kamu saat ini: M200 | Kewajiban: M600
```
```
[Jual ke Bank]
```
1. Jakarta (JKT) [MERAH] Harga Jual: M400
2. Bandung (BDG) [MERAH] Harga Jual: M375 (termasuk 1 rumah:
M75)

```
[Gadaikan]
```
3. Kupang (KPG) [UNGU] Nilai Gadai: M100

```
Pilih aksi (0 jika sudah cukup): 1
```
```
Jakarta terjual ke Bank. Kamu menerima M400.
Uang kamu sekarang: M600
```
```
Kewajiban M600 terpenuhi. Membayar ke Uname2...
Uang kamu : M600 → M0
Uang Uname2: M300 → M900
```

```
Uang kamu : M150
Total kewajiban : M1200
```
```
Estimasi dana maksimum dari likuidasi:
Jual semua properti + bangunan → M750
Total aset + uang tunai : M900
Tidak cukup untuk menutup kewajiban M1200.
```
```
Uname1 dinyatakan BANGKRUT!
Kreditor: Uname2
```
```
Pengalihan aset ke Uname2:
```
- Uang tunai sisa : M150
- Bandung (BDG) [MERAH] OWNED (1 rumah)
- Kupang (KPG) [UNGU] MORTGAGED [M]

```
Uname2 menerima semua aset Uname1.
Uname1 telah keluar dari permainan.
Permainan berlanjut dengan 3 pemain tersisa.
```
**Skenario 3: Likuidasi tidak dapat menutup kewajiban pembayaran ke bank**

```
Kamu tidak dapat membayar Pajak Barang Mewah M800!
```
```
Uang kamu : M100
Total kewajiban : M800
```
```
Estimasi dana maksimum dari likuidasi:
Jual semua properti + bangunan → M550
Total aset + uang tunai : M650
Tidak cukup untuk menutup kewajiban M800.
```
```
Uname3 dinyatakan BANGKRUT!
Kreditor: Bank
```
```
Uang sisa M100 diserahkan ke Bank.
Seluruh properti dikembalikan ke status BANK.
Bangunan dihancurkan — stok dikembalikan ke Bank.
```
```
Properti akan dilelang satu per satu:
→ Lelang: Jakarta (JKT) ...
→ Lelang: Kutai (KUT) ...
[Lelang berjalan sesuai mekanisme LELANG]
```
```
Uname3 telah keluar dari permainan.
Permainan berlanjut dengan 2 pemain tersisa.
```

**15. SIMPAN**

```
Perintah untuk menyimpan seluruh state permainan ke file. Hanya dapat dipanggil di awal
giliran pemain (belum melakukan aksi apapun).
```
### 16. MUAT...............................................................................................................................

```
Perintah untuk memuat state permainan dari file. Hanya dapat dilakukan sebelum
permainan dimulai.
```
```
> SIMPAN game_sesi1.txt
```
```
Menyimpan permainan...
Permainan berhasil disimpan ke: game_sesi1.txt
```
```
---
```
```
// Jika nama file sudah ada:
File "game_sesi1.nmp" sudah ada.
Timpa file lama? (y/n): y
File berhasil ditimpa.
```
```
---
```
```
// Jika gagal menyimpan:
Gagal menyimpan file! Pastikan direktori dapat ditulis.
```
```
> MUAT game_sesi1.txt
```
```
Memuat permainan...
```
```
Permainan berhasil dimuat. Melanjutkan giliran Uname2...
```
```
---
```
```
// Jika file tidak ditemukan:
File "game_sesi1.txt" tidak ditemukan.
```
```
---
```
```
// Jika file rusak atau format tidak dikenali:
Gagal memuat file! File rusak atau format tidak dikenali.
```

**17. CETAK_LOG**

```
Perintah untuk menampilkan entri log. Dapat diikuti argumen opsional untuk menentukan
jumlah entri terbaru yang ditampilkan. Jika tidak memakai argumen, cetak semua log.
```
```
Cetak semua log
```
```
Dengan argumen jumlah baris
> CETAK_LOG 5
```
```
=== Log Transaksi (5 Terakhir) ===
```
```
[Turn 14] Uname3 | SEWA | Bayar M10000 ke Uname2 (JKT,
festival aktif x2)
[Turn 14] Uname4 | KARTU | Pakai TeleportCard → pindah ke
Bali (BAL)
[Turn 14] Uname4 | BELI | Beli Bali (BAL) seharga M300
[Turn 15] Uname1 | DADU | Lempar: 5+2=7 → mendarat di
Stasiun (KAI)
[Turn 15] Uname1 | RAILROAD | KAI kini milik Uname1 (otomatis)
```
### 18. MENANG (OTOMATIS)...................................................................................................

```
Perintah ini dipicu saat kondisi kemenangan terpenuhi
```
##### > CETAK_LOG

```
=== Log Transaksi Penuh ===
```
```
[Turn 13] Uname1 | DADU | Lempar: 3+4=7 → mendarat di
Bandung (BDG)
[Turn 13] Uname1 | SEWA | Bayar M300 ke Uname3 (BDG, 1
rumah)
[Turn 13] Uname2 | DADU | Lempar: 6+6=12 (double) →
mendarat di Parkir
[Turn 13] Uname2 | DOUBLE | Giliran tambahan ke-1
[Turn 13] Uname2 | DADU | Lempar: 2+3=5 → mendarat di
Festival
[Turn 13] Uname2 | FESTIVAL | Jakarta (JKT): sewa M5000 →
M10000, durasi 3 giliran
[Turn 14] Uname3 | DADU | Lempar: 1+2=3 → mendarat di
Jakarta (JKT)
[Turn 14] Uname3 | SEWA | Bayar M10000 ke Uname2 (JKT,
festival aktif x2)
[Turn 14] Uname4 | KARTU | Pakai TeleportCard → pindah ke
Bali (BAL)
[Turn 14] Uname4 | BELI | Beli Bali (BAL) seharga M300
```

### 19. KESEMPATAN DAN DANA UMUM (OTOMATIS)...........................................................

```
Perintah ini dipicu saat pemain mendarat di petak kesempatan atau dana umum.
```
```
// Tampilan menang dibebaskan sesuai kreativitas kalian.
// Berikut hanya penjelasan informasi apa saja yang harus
// ditampilkan
```
```
// SKENARIO MAX TURN
// Tampilkan tanda permainan selesai
// Tampilkan semua informasi penentu kemenangan
// Tampilkan pemenangnya (bisa lebih dari satu jika seri
terus)
// Tampilkan summary permainan (opsional)
```
```
Permainan selesai! (Batas giliran tercapai)
```
```
Rekap pemain:
```
```
Pemain A
Uang : M5000
Properti : 6
Kartu : 2
```
```
Pemain B
Uang : M5000
Properti : 5
Kartu : 4
```
```
Pemain C
Uang : M4500
```
```
Pemenang: Pemain A
```
```
// SKENARIO BANKRUPTCY
// Tampilkan tanda permainan selesai
// Tampilkan pemenangnya (hanya satu)
// Tampilkan summary permainan (opsional)
Permainan selesai! (Semua pemain kecuali satu bangkrut)
```
```
Pemain tersisa:
```
- Pemain C

```
Pemenang: Pemain C
```
```
Kamu mendarat di Petak Kesempatan!
Mengambil kartu...
```

### 20. GUNAKAN KEMAMPUAN...............................................................................................

```
Perintah yang digunakan oleh pemain untuk mengaktifkan Kartu Kemampuan Spesial
yang ada di tangannya.
```
```
Kartu: "Mundur 3 petak."
Bidak dipindahkan ke IKN.
```
```
Kamu mendarat di Petak Dana Umum!
Mengambil kartu...
Kartu: "Biaya dokter. Bayar M700."
Kamu membayar M700 ke Bank. Sisa Uang = M1.500.
```
```
// Jika tidak mampu bayar:
Kamu tidak mampu membayar biaya dokter! (M700)
Uang kamu saat ini: M200
// Alur dilanjutkan ke Kebangkrutan
```
##### > GUNAKAN_KEMAMPUAN

```
Daftar Kartu Kemampuan Spesial Anda:
```
1. MoveCard - Maju 4 Petak
2. ShieldCard - Kebal tagihan atau sanksi selama 1 turn
0. Batal

```
Pilih kartu yang ingin digunakan (0-2): 2
```
```
ShieldCard diaktifkan! Anda kebal terhadap tagihan atau sanksi
selama giliran ini.
```
```
> LEMPAR_DADU
Mengocok dadu...
Hasil: 1 + 2 = 3
Memajukan Bidak Pemain1 sebanyak 3 petakk...
Bidak mendarat di: Dana Umum.
Mengambil kartu...
Kartu: "Biaya dokter. Bayar M700."
```
```
[SHIELD ACTIVE]: Efek ShieldCard melindungi Anda!
Tagihan M700 dibatalkan. Uang Anda tetap: M150.
```
```
// Skenario Sudah Menggunakan Kemampuan pada Giliran Ini
> GUNAKAN_KEMAMPUAN
```
```
Kamu sudah menggunakan kartu kemampuan pada giliran ini!
Penggunaan kartu dibatasi maksimal 1 kali dalam 1 giliran.
```

### 21. DROP KARTU KEMAMPUAN (OTOMATIS)...................................................................

```
Perintah ini dipicu secara otomatis pada awal giliran jika seorang pemain sudah memiliki 3
kartu kemampuan di tangan dan menerima kartu ke-4.
```
```
// Skenario Digunakan Setelah Lempar Dadu
> GUNAKAN_KEMAMPUAN
```
```
Kartu kemampuan hanya bisa digunakan SEBELUM melempar dadu.
```
```
Kamu mendapatkan 1 kartu acak baru!
Kartu yang didapat: TeleportCard.
PERINGATAN: Kamu sudah memiliki 3 kartu di tangan (Maksimal
3)! Kamu diwajibkan membuang 1 kartu.
```
```
Daftar Kartu Kemampuan Anda:
```
1. MoveCard - Maju 4 Petak
2. ShieldCard - Kebal tagihan/sanksi
3. LassoCard - Menarik lawan
4. TeleportCard - Pindah ke petak manapun
Pilih nomor kartu yang ingin dibuang (1-4): 3

```
LassoCard telah dibuang. Sekarang kamu memiliki 3 kartu di
tangan.
```

## Bonus..........................................................................................................................................

### Antarmuka Pengguna Grafis/Graphical User Interface (GUI).................................................................

Tampilan antarmuka berbasis teks (CLI) membuat para nimons tidak leluasa melihat visualisasi
peta di Nimonspoli. Untuk membantu para nimons menguasai kota kota di Nimonspoli, buatlah
sebuah tampilan antarmuka pengguna grafis sederhana.

Tentunya APG harus dibuat menggunakan bahasa C++ agar tetap cocok dengan sistem yang telah
ada di laboratorium para Nimons. Pastikan implementasi APG hanya untuk tampilan saja, tidak
diperbolehkan menggunakan game engine, tidak boleh mengatur game state dan sebagainya.
Tidak ada spesifikasi khusus seperti apakah tampilan antarmukanya. Kerjakan sesuai dengan
kreativitas kalian.

Keseluruhan fungsionalitas dalam aplikasi harus dapat berjalan dan diterapkan dengan baik dalam
APG tanpa terkecuali. Saran Library: Raylib, wxWidget, imgui, sfml

### COM................................................................................................................................................................................

Para nimons terkadang merasa kesepian saat tidak ada teman untuk diajak bermain Nimonspoli.
Untuk menemani dan membantu pada nimons menguji strategi terbaik mereka, buatlah sebuah
fitur pemain komputer/ _Computer Player_ (COM) yang dapat bertindak sebagai lawan main secara
otomatis. Pastikan COM mampu melakukan aksi-aksi dasar permainan secara mandiri (seperti
melempar dadu, memutuskan pembelian properti, hingga membayar sewa) tanpa adanya campur
tangan dari pemain manusia. Tidak ada spesifikasi khusus seberapa cerdas COM ini. Keseluruhan
fungsionalitas permainan harus tetap berjalan dengan lancar saat COM ikut serta, dan setiap aksi
yang diambil oleh COM harus diinformasikan dengan jelas kepada pemain, baik melalui CLI
maupun APG (jika kalian juga mengerjakan bonus tersebut).

### Papan Dinamis......................................................................................................................

Salah satu inti dari OOP adalah kemampuan mengabstraksikan sesuatu. Hal ini terkait dengan
seberapa baik kalian menggeneralisasi sebuah konsep sehingga sistem tidak terikat pada satu
bentuk yang kaku. Bonus ini hadir untuk menguji sejauh mana desain kalian benar-benar fleksibel,
bukan sekadar _hard-coded_ untuk satu konfigurasi papan.

Pada mode Papan Dinamis, susunan petak pada papan permainan tidak lagi mengikuti urutan
tetap yang telah ditentukan di spesifikasi. Sebagai gantinya, susunan petak dibaca dari sebuah file
konfigurasi tambahan yang dapat dikustomisasi. Jumlah petak tidak harus 40, dan urutan serta
jenis petak dapat diatur bebas, selama memenuhi syarat berikut:

```
● Tetap terdapat tepat 1 Petak Mulai (GO) dan 1 Petak Penjara.
```

```
● Jumlah petak minimal adalah 20 dan maksimal 60.
● Setiap properti yang direferensikan di papan harus terdefinisi di property.txt.
● Seluruh mekanisme permainan (sewa, lelang, festival, pajak, dll.) harus tetap berjalan
dengan benar di atas konfigurasi papan apapun.
```
Jika desain kelas kalian sudah cukup abstrak dan terstruktur, implementasi ini seharusnya tidak
memerlukan perombakan besar. Cukup ubah bagian yang membaca dan membangun papan. Jika
tidak, kalian akan tahu di mana letak masalahnya.

### Kreativitas....................................................................................................................................................................

Bantulah para nimons dalam memonopoli dunia Nimonspoli dengan cara membuatkan fitur
tambahan bagi mereka. Maksimal fitur kreativitas yang diimplementasikan adalah 1. Fitur yang
diimplementasikan sebaiknya tidak mengubah/mengganggu fitur yang sudah dituliskan pada
spesifikasi wajib. Tentunya nilai dari fitur kreativitas ini bergantung terhadap kompleksitas fitur
yang diimplementasikan (fitur sederhana seperti bantuan/keluar tidak dihitung yah).
Saran ide:

- Account Based & Leaderboard
- Kartu tambahan untuk Skill Cards


## Spesifikasi Sistem....................................................................................................................................................

### Ketentuan Umum.......................................................................................................................................................

Buatlah aplikasi berbasis **Antarmuka Baris Perintah (ABP) /** **_Command Line Interface_** **(CLI)** untuk
permainan ini dalam bahasa **C++** dan dapat di- _compile_ dengan memanfaatkan **makefile**.

### Ketentuan Teknis.......................................................................................................................................................

Berikut adalah hal-hal yang **minimal** wajib diimplementasikan di aplikasi yang Anda buat.
Diperbolehkan **menambah** dengan memerhatikan konsep-konsep OOP serta desain dari aplikasi
kalian!

**Perlu dicatat, yang ditulis disini hanyalah minimal dan sangat besar kemungkinan kelompok
kalian akan menambah abstraksi/konsep lainnya untuk membuat aplikasi ini.**

1. Inheritance **dan** Polymorphism
    Contoh: Tile (Petak Papan), SkillCard, serta ActionCard.
2. Exception **dan** Exception Handling
    a. Validasi uang tidak cukup saat membeli properti, membayar sewa, atau melakukan
       _bid_ lelang.
    b. Validasi slot penuh pada kepemilikan Kartu Skill di tangan maksimal 3.
    c. Validasi lainnya jika diperlukan.
3. Function Overloading **dan** Operator Overloading
    a. Penggunaan + atau += dapat digunakan untuk menambah atau memotong uang
       tunai pemain (misal: saat menerima gaji atau membayar denda).
    b. Penggunaan < atau > untuk membandingkan total kekayaan objek antar pemain
       pada penentuan klasemen pemenang di akhir batas _turn_.
4. Abstract Class **dan** Virtual Function
    Contoh: Sebuah kelas abstrak Tile yang memiliki virtual function (misalnya onLanded())
    yang akan di-inherit oleh masing-masing kelas spesifik seperti PropertyTile, UtilityTile, dan
    SpecialTile (Festival/Penjara).
5. Generic Function **atau** Generic Class
    Contoh: Generic class CardDeck<T> untuk meng-handle berbagai jenis tumpukan kartu
    yang berbeda pada permainan (seperti tumpukan Kartu Skill, kartu Chance, dan kartu
    Community Chest).
6. STL ( _Standard Template Library_ )


```
Bebas dipakai pada kelas apapun yang mendukung pembuatan gim. Wajib bersifat krusial
ke struktur data dan mekanisme permainan, bukan hanya untuk print atau diletakkan pada
sebuah kelas namun tidak terpakai. Wajib dipakai: Vector dan Map. Boleh menambah STL
lainnya jika merasa diperlukan.
```
## Panduan Pengerjaan...............................................................................................................................................

Tugas Besar ini dikerjakan secara sendiri/berkelompok _(senyaman kalian mengerjakan)_ yang berisi **5
anggota**. Silakan mendata kelompok di IF2010 OOP - Kelompok Tugas Besar 1. Serta buatlah
tim pada Github Classroom dengan nama tim merupakan kode kelompok. Tim hanya dibuat oleh
perwakilan salah satu anggota saja, sisanya bisa langsung join ke tim yang sudah dibuat

Semua link kebutuhan tugas besar ada di drive ini: Public

Adapun ketentuan lain seperti berikut:

```
1. Wajib bisa dijalankan di sistem operasi LINUX dengan C++11, C++14 atau C++17.
a. Akan ada pengurangan nilai jika terjadi anomali seperti keluaran yang tidak
muncul karena sejauh pengerjaan, kalian mengerjakan di Windows.
b. Berhak tidak dinilai jika program tidak jalan total atau gagal mensimulasikan
mayoritas kondisi untuk test case yang dibuat asisten.
c. Windows Subsystem Linux atau WSL diperbolehkan untuk substitusi sistem
operasi LINUX.
d. Dihimbau untuk setiap kelompok mengerjakan di WSL atau Linux daripada diakhir
saat demo tidak jalan sama sekali/ada kasus unik.
```
2. Hanya diperbolehkan menggunakan pustaka umum dan STL. Tuliskan justifikasi pustaka
    yang dipakai pada laporan anda.
3. Tidak boleh plagiat dari internet maupun kelompok lain.
4. Usahakan penggunaan statik, kelas dan _method_ lebih banyak (~90%) **dibandingkan** dengan
    penggunaan _global variable,_ fungsi dan prosedur_._ Pengecualian **penggunaan struct** , tidak
    diperkenankan sama sekali, silakan gunakan kelas dan objek.
5. Dekomposisi yang baik dan implementasi yang tidak terlalu kompleks (sebuah method
    tidak terlalu panjang). Pecah-pecah dan buat method baru agar tidak terlalu kompleks.
6. Dilarang menggunakan library yang memodifikasi tipe dari instance seperti variant, any,
    dynamic_cast, dll. Penggunaan dynamic cast diperbolehkan tetapi tidak boleh overuse

Selain penilaian dari tes fungsional, program akan dinilai dari segi desain arsitektur. Untuk
mendapatkan nilai yang maksimal, pastikan program Anda mematuhi prinsip prinsip desain
berikut:

**1. Implementasi OOP yang tepat!**
    Jangan sekadar menggunakan 6 konsep OOP dasar hanya untuk menggugurkan kewajiban.
    Gunakan konsep tersebut secara logis dan sesuai dengan _use case_ yang dihadapi**!**


**2. Prinsip DRY (** **_Don’t Repeat Yourself_** **)**
    Tidak boleh memiliki kode duplikat, pindahkan ke fungsi/kelas.
**3. Struktur Kelas dan** **_Layered Architecture_**
    Bangun struktur kelas dan hierarki yang rapi dan mudah dipahami alurnya. Program wajib
    mengimplementasikan _Layered Architecture_ untuk memisahkan tanggung jawab. Setidaknya
    terdiri dari 3 lapis:
       a. **User Interaction (UI) Layer** : Murni bertugas menangani input/output dari dan ke
          pengguna (layar terminal/GUI).
       b. **Game Logic (Core) Layer** : Inti aturan permainan, kalkulasi, manajemen _state_ , dll_._
          Intinya core logic nya di sini
       c. **Data Access Layer** : Murni bertugas menangani pembacaan dan penulisan file
          (seperti Konfigurasi atau fitur Save/Load)
**4. Implementasikan layered architecture: jelaskan singkat bahwa ada 3 layer (user**
    **interaction, game logic, data access),
5. Prinsip SL dari SOLID**
    **a. S (** **_Single Responsibility_** **)** :
       Sebuah kelas hanya memiliki **satu tanggung jawab.**
    **b. L (** **_Liskov Substitution_** **):**
       _Child class_ harus bisa merepresentasikan dan menggantikan _parent class_ tanpa
       merusak fungsionalitas program
**6. Terdapat “PENALTI KERAS” jika:**
    a. Apabila paradigma program masih bersifat **prosedural atau fungsional** yang hanya
       dibungkus dalam bentuk kelas agar seolah-olah terlihat seperti OOP. Semua kelas
       tidak memiliki _behavior_ , hanya memiliki _method_ run(), dan _main program_ hanya
       bertugas memanggil run() tersebut satu per satu dari atas ke bawah.
    b. **God Class,** Membangun keseluruhan program hanya dengan satu atau beberapa
       kelas besar (God Class) dan _main program_. Pastikan setiap kelas dan objek yang
       dibuat ikut berpartisipasi dan memiliki _behavior_ dalam mekanisme sistem
    c. Membuat kelas yang **tanggung jawabnya tidak relevan!**
       Contoh: Membuat kelas dengan nama anggota kelompok dan kelas
       merepresentasikan pekerjaan anggota kelompok tersebut
    _d._ **_Obfuscation_** **(Penamaan yang buruk)** terhadap nama kelas, _variable_ , _method_.
       Penamaan harus deskriptif dan memiliki arti. Contoh buruk (Kelas Foo atau Kelas
       Bar yang berisi _method_ a(), b(), c() dan memiliki _variable_ int cnt, stack<T> st,
       vector<string> vs)
    e. **Komposisi Objek yang buruk.** Misalnya, melakukan seluruh proses eksekusi logika
       di dalam konstruktor kelas. Contoh lainnya mungkin semua kelas pada program
       hanya punya method run kemudian main program memanggil run tersebut 1 per 1


## Milestone Pengumpulan.......................................................................................................................................

Tugas besar ini akan dibagi menjadi 2 Milestone. Berikut adalah penjelasan setiap
Milestone.

**Milestone 1**

Pada milestone 1, kalian wajib melakukan beberapa hal berikut:

1. Menentukan anggota kelompok dan menulisnya pada sheets yang telah ditetapkan
2. Membuat tim pada github classroom
3. Mengumpulkan desain kelas sesuai dengan format berikut
    IF2010_TB1_Laporan M1_XXX.docx. Desain kelas ini akan menjadi tolak ukur
bagaimana kalian mendesain suatu solusi persoalan berdasarkan konsep konsep OOP yang
telah dipelajari. Desain ini juga akan membantu kalian sebagai bahan acuan untuk
implementasi di M2.
Pengumpulan milestone 1 dilakukan oleh perwakilan kelompok pada link berikut sebelum **12 April
2026 pukul 23.59 WIB**

**Milestone 2**

Pada milestone 2, kalian wajib melakukan beberapa hal berikut:

1. Melakukan implementasi tugas besar secara keseluruhan
2. Mengumpulkan form asistensi
3. Melakukan dokumentasi terhadap implementasi kalian pada dokumen Laporan Tugas
    Besar. Berikut adalah format Laporan Tugas Besar IF2010_TB1_Laporan M2_XXX.docx

Laporan dan kode program dikumpulkan bersamaan dengan **_rilis_** di Github Classroom sesuai
deadline **(24 April 2026 pukul 23.59)**.

## Asistensi......................................................................................................................................................................

```
1. Asistensi sinkron wajib dilakukan minimal satu kali dengan ketentuan harus dilaksanakan
sebelum deadline pengumpulan milestone 2, maksimal menghubungi asisten 24 jam
sebelum asistensi.
```
2. Asistensi digunakan untuk bertanya mengenai spesifikasi maupun pengerjaan yang
    sekiranya belum terbayang dan belum jelas.
3. Alokasi asisten untuk tiap kelompok akan diumumkan setelah deadline pendaftaran
    kelompok.
4. Diwajibkan untuk mengisi form asistensi yang ditandatangani pula oleh asisten
    IF2010_TB1_Asistensi_XXX.docx


## QnA..............................................................................................................................................................................

Pertanyaan terkait dengan spesifikasi yang belum jelas atau hal-hal yang berkaitan dengan
masalah teknis pada tugas besar dapat ditanyakan pada sheets berikut QnA - Tugas Besar 1


