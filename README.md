1. Langkah pertama instal ekstensi platform io di vscode.
2. kemudian buat project baru dengan board generic esp8266 1mb, nanti akan muncul folder.
3. didalam folder src akan ada file main.cpp ganti semua isi file main.cpp dengan main.cpp yang ada di github ini.
4. lalu dibagian platform io juga ubah dengan kode yang saya sediakan, disitu adalah library yang kita butuhkan.
5. jika anda menggunakan arduino ide untuk compile , anda bisa copy kode main.cpp dan platformio.ini lalu tempel ke gemini dan suruh ubah ke kode untuk digunakan di arduino ide.
6. namun perlu diketahui di arduino ide untuk install librarynya ada menunya jadi nggak seperti di platformio ini.
7. kemudian ketika mau terhubung dengan aplikasi dalam jarak jauh, anda perlu hosting , nah 3 file yang berekstensi .php silahkan upload ke public_html hosting anda.
8. untuk konfigurasi api backend nya anda perlu ubah di main.cpp, dan untuk file aplikasi yang saya uplod itu sudah di set ke api backend saya, jadi anda perlu membangun ulang aplikasinya.
    
