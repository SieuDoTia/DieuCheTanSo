/* LưuÂmThanh.h */
#include "LuuAmThanhWAV.h"
#include "HangSo.h"

void luuThongTinDau( FILE *tep, unsigned int tocDoMau, unsigned char soLuongKenh, unsigned short kieuDuLieu );

void luuDuLieuSoNguyen16bit_nhiKenh( FILE *tep, DuLieuAmThanh_short *duLieuAmThanh );
void luuDuLieuSoThat32bit_nhiKenh( FILE *tep, DuLieuAmThanh_float *duLieuAmThanh );

#pragma mark ---- LƯU ÂM THANH
void luuAmThanhWAV_float( char *tenTep, DuLieuAmThanh_float *duLieuAmThanh ) {

   // ---- mở tệp
   FILE *tep  = fopen(  tenTep, "wb" );
   
   if( tep ) {
      // ---- "RIFF"
      fprintf( tep, "RIFF" );
      
      // ---- bề dài âm thanh tệp - 8
      unsigned int beDai = 32;
      beDai += ((duLieuAmThanh->soLuongMau)*(duLieuAmThanh->soLuongKenh)) << 2; // 4 byte cho float
      printf( "LuuAmThanhWAV_float:  beDai %d   duLieuAmThanh->soLuongMau %d\n", beDai, duLieuAmThanh->soLuongMau );

      // ---- lưu bề dài
      fputc( (beDai) & 0xff, tep );
      fputc( (beDai >> 8) & 0xff, tep );
      fputc( (beDai >> 16) & 0xff, tep );
      fputc( (beDai >> 24) & 0xff, tep );
      
      // ---- "WAVE"
      fprintf( tep, "WAVE" );
      
      // ---- lưu đầu tệp
      luuThongTinDau( tep, duLieuAmThanh->tocDoMau, duLieuAmThanh->soLuongKenh, kKIEU_FLOAT );
      
      // ---- lưu dữ liệu tệp
      if( duLieuAmThanh->soLuongKenh == kNHI_KENH )
         luuDuLieuSoThat32bit_nhiKenh( tep, duLieuAmThanh );
      else {
         printf( "LuuAmThanhWAV_float: Chỉ hỗ trợ NHỊ_KÊNH\n" );
//         exit(0);
      }
         
      
      // ---- đống tệp
      fclose( tep );
   }
   else {
      printf( "LuuAmThanhWAV_float: Vấn đề lưu tệp %s\n", tenTep );
   }
}

void luuAmThanhWAV_short( char *tenTep, DuLieuAmThanh_short *duLieuAmThanh ) {
   
   // ---- mở tệp
   FILE *tep  = fopen(  tenTep, "wb" );
   
   if( tep ) {
      // ---- "RIFF"
      fprintf( tep, "RIFF" );
      
      // ---- bề dài âm thanh tệp - 8
      unsigned int beDai = 32;
      beDai += ((duLieuAmThanh->soLuongMau)*(duLieuAmThanh->soLuongKenh)) << 1; // 2 byte cho short

      printf( "LuuAmThanhWAV_short:  beDai %d   duLieuAmThanh->soLuongMau %d\n", beDai, duLieuAmThanh->soLuongMau );
      
      // ---- lưu bề dài
      fputc( (beDai) & 0xff, tep );
      fputc( (beDai >> 8) & 0xff, tep );
      fputc( (beDai >> 16) & 0xff, tep );
      fputc( (beDai >> 24) & 0xff, tep );
      
      // ---- "WAVE"
      fprintf( tep, "WAVE" );
      
      // ---- lưu đầu tệp
      luuThongTinDau( tep, duLieuAmThanh->tocDoMau, duLieuAmThanh->soLuongKenh, kKIEU_SHORT );
      
      // ---- lưu dữ liệu tệp
      if( duLieuAmThanh->soLuongKenh == kNHI_KENH )
         luuDuLieuSoNguyen16bit_nhiKenh( tep, duLieuAmThanh );
      else {
         printf( "LuuAmThanhWAV_short: Chỉ hỗ trợ NHỊ_KÊNH\n" );
         //         exit(0);
      }
      
      
      // ---- đống tệp
      fclose( tep );
   }
   else {
      printf( "LuuAmThanhWAV_short: Vấn đề lưu tệp %s\n", tenTep );
   }
}

void luuThongTinDau( FILE *tep, unsigned int tocDoMau, unsigned char soLuongKenh, unsigned short kieuDuLieu ) {

   // ---- tên cho đầu "fmt "
   fprintf( tep, "fmt " );

   // ---- bề dài
   fputc( 0x10, tep );  // 16 byte
   fputc( 0x00, tep );
   fputc( 0x00, tep );
   fputc( 0x00, tep );

   // ---- phương pháp mã hóa
   fputc( kieuDuLieu & 0xff, tep );
   fputc( (kieuDuLieu >> 8) & 0xff, tep );

   // ---- sô lượng kênh
   fputc( soLuongKenh & 0xff, tep );
   fputc( (soLuongKenh >> 8) & 0xff, tep );

   // ---- tốc độ mẫu vật (Hz)
   fputc( tocDoMau & 0xff, tep );
   fputc( (tocDoMau >> 8) & 0xff, tep );
   fputc( (tocDoMau >> 16) & 0xff, tep );
   fputc( (tocDoMau >> 24) & 0xff, tep );
   
   // ---- số lượng byte/giây
   unsigned int soLuongByteGiay = tocDoMau;
   if( kieuDuLieu == kKIEU_SHORT )
      soLuongByteGiay *= soLuongKenh << 1;
   else if( kieuDuLieu == kKIEU_FLOAT )
      soLuongByteGiay *= soLuongKenh << 2;

   fputc( soLuongByteGiay & 0xff, tep );
   fputc( (soLuongByteGiay >> 8) & 0xff, tep );
   fputc( (soLuongByteGiay >> 16) & 0xff, tep );
   fputc( (soLuongByteGiay >> 24) & 0xff, tep );

   // ---- byte cho một cục (nên = số lượng mẫu vật * số lượng kênh)
  // ---- số lượng byte mẫu vật
   unsigned short byteKhungMau = 0;
   unsigned char soLuongBitMau = 0;
   if( kieuDuLieu == kKIEU_SHORT ) {
       byteKhungMau = soLuongKenh << 1;
       soLuongBitMau = 16;
   }
   else if( kieuDuLieu == kKIEU_FLOAT ) {
      byteKhungMau = soLuongKenh << 2;
      soLuongBitMau = 32;
   }
   else 
      printf( "LuuAmThanhWAV: Sai lầm: kiểu dữ liệu lạ: %x\n", kieuDuLieu );

   fputc( byteKhungMau & 0xff, tep );
   fputc( (byteKhungMau >> 8) & 0xff, tep );
   fputc( soLuongBitMau, tep );
   fputc( 0x00, tep );
}
   
void luuDuLieuSoNguyen16bit_nhiKenh( FILE *tep, DuLieuAmThanh_short *duLieuAmThanh ) {

   fprintf( tep, "data" );
   
   // ---- tính số lượng mẫu vật
   unsigned int soLuongByte = duLieuAmThanh->soLuongMau;
   soLuongByte *= duLieuAmThanh->soLuongKenh << 1;   // nhị kênh, 2 byte
   
   fputc( soLuongByte & 0xff, tep );
   fputc( (soLuongByte >> 8) & 0xff, tep );
   fputc( (soLuongByte >> 16) & 0xff, tep );
   fputc( (soLuongByte >> 24) & 0xff, tep );

   unsigned int soLuongMau = (duLieuAmThanh->soLuongMau) << 1;
   unsigned int chiSoMau = 0;
   while( chiSoMau < soLuongMau ) {

      short duLieuShort = duLieuAmThanh->mangMau[chiSoMau];
      fputc( duLieuShort & 0xff, tep );
      fputc( (duLieuShort >> 8) & 0xff, tep );
      duLieuShort = duLieuAmThanh->mangMau[chiSoMau+1];
      fputc( duLieuShort & 0xff, tep );
      fputc( (duLieuShort >> 8) & 0xff, tep );

      chiSoMau += 2;
   }

}

void luuDuLieuSoThat32bit_nhiKenh( FILE *tep, DuLieuAmThanh_float *duLieuAmThanh ) {

   fprintf( tep, "data" );

   // ---- tính số lượng mẫu vật
   unsigned int soLuongByte = duLieuAmThanh->soLuongMau;
   soLuongByte *= duLieuAmThanh->soLuongKenh << 2;  // nhị kênh, 4 byte

   fputc( soLuongByte & 0xff, tep );
   fputc( (soLuongByte >> 8) & 0xff, tep );
   fputc( (soLuongByte >> 16) & 0xff, tep );
   fputc( (soLuongByte >> 24) & 0xff, tep );
   
   unsigned int soLuongMau = duLieuAmThanh->soLuongMau << 1;
   unsigned int chiSoMau = 0;
   while( chiSoMau < soLuongMau ) {
      union {
         float f;
         unsigned int i;
      } ui;
      
      ui.f = duLieuAmThanh->mangMau[chiSoMau];
      fputc( ui.i & 0xff, tep );
      fputc( (ui.i >> 8) & 0xff, tep );
      fputc( (ui.i >> 16) & 0xff, tep );
      fputc( (ui.i >> 24) & 0xff, tep );
      
      ui.f = duLieuAmThanh->mangMau[chiSoMau+1];
      fputc( ui.i & 0xff, tep );
      fputc( (ui.i >> 8) & 0xff, tep );
      fputc( (ui.i >> 16) & 0xff, tep );
      fputc( (ui.i >> 24) & 0xff, tep );

      chiSoMau += 2;
   }
}