// 2020.09.16 ĐiềuChếTầnSố short
// 2020.10.20 Phiên Bản YM2151

// Hệ thống có bốn 'giao động' (phát sóng) cho mỗi kênh
// Mỗi kênh có hai bộ Bộ Âm lượng và Bộ Tần Số chứa thông tin về bốn giao động:
//    Hệ Số Bộ Âm lượng
//      - loạiChỉnh
//      - nhịpChỉnh - chỉ cho chỉnh ngẫu nhiên
//      - tầnSốChỉnh

//      - trungBình m1
//      - trungBình m2
//      - trungBình c1
//      - trungBình c2

//      - biênĐồ m1
//      - biênĐồ m2
//      - biênĐồ c1
//      - biênĐồ c2

//    Hệ Số Bộ Tần Số
//      - loạiChinh
//      - tầnSốChỉnh

//      - trungBình m1
//      - trungBình m2
//      - trungBình c1
//      - trungBình c2

//      - biênĐồ m1
//      - biênĐồ m2
//      - biênĐồ c1
//      - biênĐồ c2

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "HangSo.h"
#include "BangSin.h"
#include "BangAmLuong.h"
#include "DuLieuAmThanh.h"
#include "LuuAmThanhWAV.h"

//#define k16_SANG_FLOAT  0.00065714121
#define kPI      3.1415926f
#define kHAI_PI  6.2831852f

#define kCHINH__LOAI_KHONG_CHINH  0  // không chỉnh tần số
#define kCHINH__LOAI_TAM_GIAC     2  // tam giác
#define kCHINH__LOAI_HUYEN_NAO    3  // huyên náo
#define kCHINH__LOAI_PARABOL      4  // parabol - bậc hai
#define kCHINH__LOAI_DOC          5  // dốc - bậc một

#define kCHINH__NHIP_8_MAU   0x00f   // 16 vì nhị kênh
#define kCHINH__NHIP_16_MAU  0x01f   // 32 vì nhị kênh
#define kCHINH__NHIP_32_MAU  0x03f   // 64 vì nhị kênh
#define kCHINH__NHIP_64_MAU  0x07f   // 128 vì nhị kênh
#define kCHINH__NHIP_128_MAU 0x0ff   // 256 vì nhị kênh
#define kCHINH__NHIP_256_MAU 0x1ff   // 512 vì nhị kênh

// ---- hàm cho bao bì âm thanh
#define kBAO_BI__HANG_SO  0
#define kBAO_BI__LUY_THUA 1
#define kBAO_BI__COS      2

// ----
//short bangSin[1024];
//short bangAmLuong[6656];   // 13*512 = 6656
#define kBE_DAI_BAN_AM_LUONG   6656 

// ---- các hàm tạo bảng
void chuanBiBangSin();
void chuanBiBangAmLuong();

// ----
float tinhTongAmLuongChoLoaiBaoBi( unsigned char loaiBaoBi, float tanSo, float thoiGian );

// ----
short tinhPhanHoi( short *phanHoi_truoc, short *phanHoi_hienTai, short phaPhanHoi, unsigned int amLuong_m1, unsigned int pha_m1 );

// ----
short tinhM1( int pha, unsigned short amLuong, short phanHoi );
short tinhCM12( int pha, unsigned short amLuong, short chinh );


// ----
void xoaMauInt( int *mangMau, unsigned int soLuongMau );

// ----
void donViHoaLanShort( int *lan,  unsigned int soLuongMau, float muc );
void phaTronHetLanVaoLan0( DuLieuAmThanh_short *duLieuAmThanh );

// ----
float doc( float viTriTinh, float viTriDau, float viTriCuoi, float doCaoDau, float doCaoCuoi );
float songTamGiacf( float giaTri );
float dayParabol( float viTriTinh, float viTriDau, float viTriCuoi, float doCaoDau, float doCaoCuoi );

// ----
void hatViDuTanSoDanChuong4( DuLieuAmThanh_short *demPhatAm );

typedef struct {
   
   unsigned char loaiChinh;
   unsigned short nhipChinh;  // chỉ cho đổi ngẫu nhiên
   float tanSoChinh;   // Hz - tần số chỉnh
   
   // ---- giá trị trung bình
   int m1;
   int m2;
   int c1;
   int c2;
   
   // ---- biên độ
   int bienDo_m1;
   int bienDo_m2;
   int bienDo_c1;
   int bienDo_c2;
   
} HeSoBo_short;

void chinhHeSo( HeSoBo_short *bo, unsigned char loaiChinh, unsigned int soMau, float thoiGian,
               unsigned int *giaTri_m1, unsigned int *giaTri_m2, unsigned int *giaTri_c1, unsigned int *giaTri_c2 );

// ----
typedef struct {
   
   unsigned char giaiThuat;   // giải thuật điều chế tần số
   float tongAmLuong;         // tổng âm lượng
   unsigned char phanHoi;
   
   unsigned char loaiBaoBi;   // loại bao bì
   float tanSoBaoBi;          // tần số bao bì

   
   // ---- âm lượng
   HeSoBo_short amLuong;

   // ---- tân số
   HeSoBo_short tanSo;
   
} DieuCheTanSo_short;


// -----
typedef struct {
   
   short phanHoi;
   short phaPhanHoi;
   short phanHoi_truoc;
   short phanHoi_hienTai;
   
} PhanHoi;


void giaiThuat0_short( int *demAm, unsigned int tocDoMau, unsigned int soMau, DieuCheTanSo_short *dan, float thoiGianHat );
void giaiThuat2_short( int *demAm, unsigned int tocDoMau, unsigned int soMau, DieuCheTanSo_short *dan, float thoiGianHat );
void giaiThuat3_short( int *demAm, unsigned int tocDoMau, unsigned int soMau, DieuCheTanSo_short *dan, float thoiGianHat );
void giaiThuat4_short( int *demAm, unsigned int tocDoMau, unsigned int soMau, DieuCheTanSo_short *dan, float thoiGianHat );
void giaiThuat5_short( int *demAm, unsigned int tocDoMau, unsigned int soMau, DieuCheTanSo_short *dan, float thoiGianHat );
void giaiThuat6_short( int *demAm, unsigned int tocDoMau, unsigned int soMau, DieuCheTanSo_short *dan, float thoiGianHat );
void giaiThuat7_short( int *demAm, unsigned int tocDoMau, unsigned int soMau, DieuCheTanSo_short *dan, float thoiGianHat );

int main( int argc, char **argv) {
   
   // chuản bị hai bảng
//   chuanBiBangSin();
//   chuanBiBangAmLuong();

   // ---- tạo đệm
   float thoiGian = 8.0f;  // 8 giây cho thử nghiệm
   unsigned int soMau = thoiGian*kTAN_SO_MAU_SHORT;
   
   DuLieuAmThanh_short duLieuAmThanh;
   duLieuAmThanh.soLuongMau = soMau;
   duLieuAmThanh.tocDoMau = kTAN_SO_MAU_SHORT;
   duLieuAmThanh.soLuongKenh = kNHI_KENH;
   duLieuAmThanh.mangMau = calloc( soMau*kNHI_KENH*kSO_LUONG__LAN, sizeof( short ) );
   duLieuAmThanh.lan0 = duLieuAmThanh.mangMau;
   duLieuAmThanh.lan1 = duLieuAmThanh.lan0 + soMau*kNHI_KENH;
   duLieuAmThanh.lan2 = duLieuAmThanh.lan1 + soMau*kNHI_KENH;

   printf( "main: %d  soMau*kNHI_KENH*sizeof( float ) %ld\n", soMau, soMau*kNHI_KENH*sizeof( float ) );
   
   float mangTanSo[] = {
       0.125f, 0.25f, 0.33333f, 0.5f,
       0.66667f, 0.75f, 1.0f, 1.16667f,
       1.33333f, 1.5f, 1.66667f, 2.0f,
       3.0f, 4.0f, 6.0f, 8.0f };

   if( duLieuAmThanh.mangMau != NULL ) {
      float chinhTanSo;
      duLieuAmThanh.soLuongMau = 2.0f*kTAN_SO_MAU_SHORT;
      DieuCheTanSo_short goGo;    // gõ gỗ - chưa xong
      goGo.tongAmLuong = 1.0f;
      goGo.giaiThuat = 4;
      goGo.phanHoi = 0;   // 13
      
      goGo.tongAmLuong = 1.0f;
      goGo.amLuong.m1 = 280;
      goGo.amLuong.m2 = 280;
      goGo.amLuong.c1 = 128;
      goGo.amLuong.c2 = 128;
      goGo.amLuong.bienDo_m1 = 202;
      goGo.amLuong.bienDo_m2 = 202;
      goGo.amLuong.bienDo_c1 = 51;
      goGo.amLuong.bienDo_c2 = 51;
      goGo.amLuong.loaiChinh = kCHINH__LOAI_DOC;
      goGo.amLuong.tanSoChinh = 4.0f;
      
      goGo.tanSo.m1 = 840384*3.0;
      goGo.tanSo.m2 = 837696*3.0;
      goGo.tanSo.c1 = 280128*3.0;
      goGo.tanSo.c2 = 279232*3.0;
      goGo.tanSo.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
//      giaiThuat4_short( duLieuAmThanh.mangMau, kTAN_SO_MAU_SHORT, soMau, &goGo, 2.0f );
//      donViHoaLanShort( duLieuAmThanh.lan0, soMau * kNHI_KENH, 1.0f );
//      luuAmThanhWAV_short( "GoGo_D.wav", &duLieuAmThanh );
      DieuCheTanSo_short chuong5;    // chuông 6
      chuong5.giaiThuat = 4;
      
      chuong5.tongAmLuong = 1.0f;
      chuong5.phanHoi = 0;
      
      chuong5.amLuong.m1 = 392;
      chuong5.amLuong.m2 = 392;
      chuong5.amLuong.c1 = 237;
      chuong5.amLuong.c2 = 237;
      chuong5.amLuong.loaiChinh = kCHINH__LOAI_DOC;
      chuong5.amLuong.tanSoChinh = 8.0f;
      chuong5.amLuong.bienDo_m1 = 0;
      chuong5.amLuong.bienDo_m2 = 0;
      chuong5.amLuong.bienDo_c1 = -49;
      chuong5.amLuong.bienDo_c2 = -49;
   
      chuong5.tanSo.m1 = 2234880*2.0f;
      chuong5.tanSo.m2 = 2240000*2.0f;
      chuong5.tanSo.c1 = 1117440*2.0f;
      chuong5.tanSo.c2 = 1120000*2.0f;
      chuong5.tanSo.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
//      giaiThuat4_short( duLieuAmThanh.mangMau, kTAN_SO_MAU_SHORT, soMau, &chuong5, 2.0f );
//      donViHoaLanShort( duLieuAmThanh.lan0, soMau * kNHI_KENH, 1.0f );
//      luuAmThanhWAV_short( "chuong5_E.wav", &duLieuAmThanh );
      DieuCheTanSo_short chuong6;    // dây 6
      chuong6.giaiThuat = 4;
      
      chuong6.tongAmLuong = 1.0f;
      chuong6.phanHoi = 0;
      
      chuong6.amLuong.m1 = 384;
      chuong6.amLuong.m2 = 384;
      chuong6.amLuong.c1 = 189;
      chuong6.amLuong.c2 = 189;
      chuong6.amLuong.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
      
      chuong6.tanSo.m1 = 1580032*2.0f;
      chuong6.tanSo.m2 = 1584128*2.0f;
      chuong6.tanSo.c1 = 790016*2.0f;
      chuong6.tanSo.c2 = 792064*2.0f;
      chuong6.tanSo.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
//      giaiThuat4_short( duLieuAmThanh.mangMau, kTAN_SO_MAU_SHORT, soMau, &chuong6, 2.0f );
//      donViHoaLanShort( duLieuAmThanh.lan0, soMau * kNHI_KENH, 1.0f );
//      luuAmThanhWAV_short( "chuong6_E.wav", &duLieuAmThanh );
      
      DieuCheTanSo_short day0;    // dây 1
      day0.giaiThuat = 4;
      
      day0.tongAmLuong = 1.0f;
      day0.phanHoi = 0;
      
      day0.amLuong.m1 = 408;
      day0.amLuong.m2 = 192;
      day0.amLuong.c1 = 192;
      day0.amLuong.c2 = 40;
      day0.amLuong.loaiChinh = kCHINH__LOAI_DOC;
      day0.amLuong.tanSoChinh = 4.0f;
      day0.amLuong.bienDo_m1 = 275;
      day0.amLuong.bienDo_m2 = 24;
      day0.amLuong.bienDo_c1 = 96;
      day0.amLuong.bienDo_c2 = 24;
      
      day0.tanSo.m1 = 1763520*8.0f;
      day0.tanSo.m2 = 58784*8.0f;
      day0.tanSo.c1 = 117568*8.0f;
      day0.tanSo.c2 = 58784*8.0f;
      day0.tanSo.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
//      giaiThuat3_short( duLieuAmThanh.mangMau, kTAN_SO_MAU_SHORT, soMau, &day0, 2.0f );
//      donViHoaLanShort( duLieuAmThanh.lan0, soMau * kNHI_KENH, 1.0f );
//      luuAmThanhWAV_short( "day_0M.wav", &duLieuAmThanh );
      
      DieuCheTanSo_short chuong7;    // chuông 7
      chuong7.giaiThuat = 4;
      
      chuong7.tongAmLuong = 1.0f;
      chuong7.phanHoi = 0;
      
      chuong7.amLuong.m1 = 216;
      chuong7.amLuong.m2 = 225;
      chuong7.amLuong.c1 = 72;
      chuong7.amLuong.c2 = 72;
      chuong7.amLuong.loaiChinh = kCHINH__LOAI_DOC;
      chuong7.amLuong.tanSoChinh = 4.0f;
      chuong7.amLuong.bienDo_m1 = 142;
      chuong7.amLuong.bienDo_m2 = 1022;
      chuong7.amLuong.bienDo_c1 = 891;
      chuong7.amLuong.bienDo_c2 = 222;
      
      chinhTanSo = 1.66667f;
      chuong7.tanSo.m1 = 940544*chinhTanSo;
      chuong7.tanSo.m2 = 2821632*chinhTanSo;
      chuong7.tanSo.c1 = 470272*chinhTanSo;
      chuong7.tanSo.c2 = 470272*chinhTanSo;
      chuong7.tanSo.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
/*      giaiThuat4_short( duLieuAmThanh.mangMau, kTAN_SO_MAU_SHORT, soMau, &chuong7, 1.0f );
      duLieuAmThanh.soLuongMau = 1.0f*kTAN_SO_MAU_SHORT;
      donViHoaLanShort( duLieuAmThanh.lan0, soMau * kNHI_KENH, 1.0f );
      luuAmThanhWAV_short( "chuong_7__.wav", &duLieuAmThanh );
      exit(0);
      */
      DieuCheTanSo_short ghen2;    // ghện 3
      ghen2.tongAmLuong = 0.5f;
      ghen2.giaiThuat = 4;
      ghen2.phanHoi = 13;   // <---- 13
      
      ghen2.amLuong.m1 = 352;
      ghen2.amLuong.m2 = 280;
      ghen2.amLuong.c1 = 280;
      ghen2.amLuong.c2 = 128;
      ghen2.amLuong.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
      
      ghen2.tanSo.m1 = 279040;
      ghen2.tanSo.m2 = 70080;
      ghen2.tanSo.c1 = 140160;
      ghen2.tanSo.c2 = 69760;
      ghen2.tanSo.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
   
//      giaiThuat4_short( duLieuAmThanh.mangMau, kTAN_SO_MAU_SHORT, soMau, &ghen2, 8.0f );
//      donViHoaLanShort( duLieuAmThanh.lan0, soMau * kNHI_KENH, 1.0f );
//      luuAmThanhWAV_short( "Ghen2.wav", &duLieuAmThanh );
//      exit(0);
      
      duLieuAmThanh.soLuongMau = 8.0f*kTAN_SO_MAU_SHORT;
      DieuCheTanSo_short day6;    // gõ gỗ - chưa xong
      day6.giaiThuat = 4;
      day6.tongAmLuong = 1.0f;
      day6.phanHoi = 7;   // <---- 7
      
      day6.amLuong.m1 = 229;
      day6.amLuong.m2 = 189;
      day6.amLuong.c1 = 293;
      day6.amLuong.c2 = 253;
      day6.amLuong.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
      
      day6.tanSo.m1 = 2844864;
      day6.tanSo.m2 = 947328;
      day6.tanSo.c1 = 1894656;
      day6.tanSo.c2 = 947904;
      day6.tanSo.loaiChinh = kCHINH__LOAI_KHONG_CHINH;

//      giaiThuat4_short( duLieuAmThanh.mangMau, kTAN_SO_MAU_SHORT, soMau, &day6, 8.0f );
      // ----
      day6.amLuong.m1 = 230;
      day6.amLuong.m2 = 190;
      day6.amLuong.c1 = 214;
      day6.amLuong.c2 = 174;
      
      day6.tanSo.m1 = 3382272;
      day6.tanSo.m2 = 1126272;
      day6.tanSo.c1 = 2252544;
      day6.tanSo.c2 = 1126848;
//      giaiThuat4_short( duLieuAmThanh.mangMau, kTAN_SO_MAU_SHORT, soMau, &day6, 8.0f );

      // ----
      day6.amLuong.m1 = 230;
      day6.amLuong.m2 = 190;
      day6.amLuong.c1 = 206;
      day6.amLuong.c2 = 167;
      
      day6.tanSo.m1 = 4262976;
      day6.tanSo.m2 = 1419840;
      day6.tanSo.c1 = 2839680;
      day6.tanSo.c2 = 1420416;

//      giaiThuat4_short( duLieuAmThanh.mangMau, kTAN_SO_MAU_SHORT, soMau, &day6, 8.0f );
//      donViHoaLanShort( duLieuAmThanh.lan0, soMau * kNHI_KENH, day6.tongAmLuong );
//      luuAmThanhWAV_short( "Day6 A.wav", &duLieuAmThanh );
//      exit(0);
      
      DieuCheTanSo_short day7;    // gõ gỗ - chưa xong
      day7.giaiThuat = 0;
      day7.tongAmLuong = 1.0f;
      day7.phanHoi = 7;   // <---- 7
      
      day7.amLuong.m1 = 216;
      day7.amLuong.m2 = 160;
      day7.amLuong.c1 = 288;
      day7.amLuong.c2 =  96;
      day7.amLuong.loaiChinh = kCHINH__LOAI_DOC;
      day7.amLuong.tanSoChinh = 4.0f;
      day7.amLuong.bienDo_m1 = 130;
      day7.amLuong.bienDo_m2 = 23;
      day7.amLuong.bienDo_c1 = 109;
      day7.amLuong.bienDo_c2 = 13;
      
      chinhTanSo = 16.0f;
      day7.tanSo.m1 = 628608*chinhTanSo;
      day7.tanSo.m2 = 52384*chinhTanSo;
      day7.tanSo.c1 = 523840*chinhTanSo;
      day7.tanSo.c2 = 104768*chinhTanSo;
      day7.tanSo.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
      duLieuAmThanh.soLuongMau = 2.0f*kTAN_SO_MAU_SHORT;      
//      giaiThuat0_short( duLieuAmThanh.mangMau, kTAN_SO_MAU_SHORT, soMau, &day7, 2.0f );
//      donViHoaLanShort( duLieuAmThanh.lan0, soMau * kNHI_KENH, 1.0f );
//      luuAmThanhWAV_short( "Day 7L.wav", &duLieuAmThanh );
//      exit(0);
      
      DieuCheTanSo_short tron1;    // trốn 1
      tron1.giaiThuat = 2;
      duLieuAmThanh.soLuongLan = 1;
      
      tron1.tongAmLuong = 1;
      tron1.phanHoi = 0;
      
      tron1.amLuong.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
      tron1.amLuong.m1 = 40;//146;
      tron1.amLuong.m2 = 152;//170;
      tron1.amLuong.c1 = 312;//322;
      tron1.amLuong.c2 = 104;//176;
      tron1.amLuong.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
      
      tron1.tanSo.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
      tron1.tanSo.m1 = 78912;
      tron1.tanSo.m2 = 158080;
      tron1.tanSo.c1 = 236928;
      tron1.tanSo.c2 = 78912;
      tron1.tanSo.loaiChinh = kCHINH__LOAI_KHONG_CHINH;

//      giaiThuat2_short( duLieuAmThanh.lan0, kTAN_SO_MAU_SHORT, soMau, &tron1, 2.0f );
//      duLieuAmThanh.soLuongMau = 2.0f*kTAN_SO_MAU_SHORT;
//      donViHoaLanShort( duLieuAmThanh.lan0, soMau * kNHI_KENH, 1.0f );

    //  luuAmThanhWAV_short( "tron1_A.wav", &duLieuAmThanh );
      
      DieuCheTanSo_short tron3;    // trốn 3
      tron3.giaiThuat = 0;
      duLieuAmThanh.soLuongLan = 1;
      
      tron3.tongAmLuong = 1;
      tron3.phanHoi = 0;//13;
      
      tron3.amLuong.m1 = 80;
      tron3.amLuong.m2 = 266;
      tron3.amLuong.c1 = 38;
      tron3.amLuong.c2 = 33;
      tron3.amLuong.loaiChinh = kCHINH__LOAI_DOC;
      tron3.amLuong.tanSoChinh = 4.0f;
      tron3.amLuong.bienDo_m1 = 755;
      tron3.amLuong.bienDo_m2 = 79;
      tron3.amLuong.bienDo_c1 = 443;
      tron3.amLuong.bienDo_c2 = 189;
   
      chinhTanSo = 1.0f;
      tron3.tanSo.m1 = 166144*chinhTanSo;
      tron3.tanSo.m2 = 41536*chinhTanSo;
      tron3.tanSo.c1 = 124608*chinhTanSo;
      tron3.tanSo.c2 = 41536*chinhTanSo;
      tron3.tanSo.loaiChinh = kCHINH__LOAI_DOC;
      tron3.tanSo.tanSoChinh = 4.0f;
      tron3.tanSo.bienDo_m1 = -83200*chinhTanSo;
      tron3.tanSo.bienDo_m2 = -20800*chinhTanSo;
      tron3.tanSo.bienDo_c1 = -62400*chinhTanSo;
      tron3.tanSo.bienDo_c2 = -28000*chinhTanSo;
      
//      giaiThuat0_short( duLieuAmThanh.lan0, kTAN_SO_MAU_SHORT, soMau, &tron3, 2.0f );
//      duLieuAmThanh.soLuongMau = 2.0f*kTAN_SO_MAU_SHORT;
//      donViHoaLanShort( duLieuAmThanh.lan0, soMau * kNHI_KENH, 1.0f );
//      luuAmThanhWAV_short( "tron3_C.wav", &duLieuAmThanh );
      
      DieuCheTanSo_short tron4;    // trốn 3
      tron4.giaiThuat = 3;
      duLieuAmThanh.soLuongLan = 1;
      
      tron4.tongAmLuong = 1;
      tron4.phanHoi = 0;//13;
      
      tron4.amLuong.m1 = 288;
      tron4.amLuong.m2 = 168;
      tron4.amLuong.c1 = 192;
      tron4.amLuong.c2 = 92;
      tron4.amLuong.loaiChinh = kCHINH__LOAI_DOC;
      tron4.amLuong.tanSoChinh = 3.0f;
      tron4.amLuong.bienDo_m1 = 216;
      tron4.amLuong.bienDo_m2 = 10;
      tron4.amLuong.bienDo_c1 = 114;
      tron4.amLuong.bienDo_c2 = 24;
      
      chinhTanSo = 4.0f;
      tron4.tanSo.m1 = 2494080*chinhTanSo;
      tron4.tanSo.m2 = 83136*chinhTanSo;
      tron4.tanSo.c1 = 665600*chinhTanSo;
      tron4.tanSo.c2 = 83136*chinhTanSo;
      tron4.tanSo.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
      
//      giaiThuat3_short( duLieuAmThanh.lan0, kTAN_SO_MAU_SHORT, soMau, &tron4, 4.0f );
//      duLieuAmThanh.soLuongMau = 4.0f*kTAN_SO_MAU_SHORT;
//      donViHoaLanShort( duLieuAmThanh.lan0, soMau * kNHI_KENH, 1.0f );
//      luuAmThanhWAV_short( "tron4_I.wav", &duLieuAmThanh );
      
      DieuCheTanSo_short ren3;    // trốn 3
      ren3.giaiThuat = 3;
      duLieuAmThanh.soLuongLan = 1;
      
      ren3.tongAmLuong = 1;
      ren3.phanHoi = 13;
      
      ren3.amLuong.m1 = 120;
      ren3.amLuong.m2 = 120;
      ren3.amLuong.c1 = 120;
      ren3.amLuong.c2 = 120;
      ren3.amLuong.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
      
      chinhTanSo = 1.0f;
      ren3.tanSo.m1 = 3110912*chinhTanSo;
      ren3.tanSo.m2 = 3110912*chinhTanSo;
      ren3.tanSo.c1 = 3110912*chinhTanSo;
      ren3.tanSo.c2 = 1555456*chinhTanSo;
      ren3.tanSo.loaiChinh = kCHINH__LOAI_HUYEN_NAO;
      ren3.tanSo.nhipChinh = kCHINH__NHIP_64_MAU;
      ren3.tanSo.bienDo_m1 = 1329152*chinhTanSo;
      ren3.tanSo.bienDo_m2 = 1329152*chinhTanSo;
      ren3.tanSo.bienDo_c1 = 1329152*chinhTanSo;
      ren3.tanSo.bienDo_c2 = 664576*chinhTanSo;
      
      
      DieuCheTanSo_short ren4;    // huyên náo
      ren4.giaiThuat = 7;
      duLieuAmThanh.soLuongLan = 1;
      
      ren4.tongAmLuong = 1;
      ren4.phanHoi = 0;
      
      ren4.amLuong.m1 = 335;
      ren4.amLuong.m2 = 258;
      ren4.amLuong.c1 = 452;
      ren4.amLuong.c2 = 304;
      ren4.amLuong.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
      ren4.loaiBaoBi = kBAO_BI__COS;
      ren4.tanSoBaoBi = 3.1415926f*0.6667f;
      
      chinhTanSo = 1.0f;
      ren4.tanSo.m1 = 3512320;//1564672*chinhTanSo;
      ren4.tanSo.m2 = 1582080;//704512*chinhTanSo;
      ren4.tanSo.c1 = 13424640;//5978112*chinhTanSo;
      ren4.tanSo.c2 = 1118720;//498176*chinhTanSo;
      ren4.tanSo.loaiChinh = kCHINH__LOAI_HUYEN_NAO;
      ren4.tanSo.nhipChinh = kCHINH__NHIP_8_MAU;
      ren4.tanSo.bienDo_m1 = 220000;//1656833*chinhTanSo;
      ren4.tanSo.bienDo_m2 = 320000;//745729*chinhTanSo;
      ren4.tanSo.bienDo_c1 = 220000;//6331393*chinhTanSo;
      ren4.tanSo.bienDo_c2 = 22000;//527617*chinhTanSo;

      giaiThuat7_short( duLieuAmThanh.lan0, kTAN_SO_MAU_SHORT, soMau, &ren4, 3.0f );
      duLieuAmThanh.soLuongMau = 3.0f*kTAN_SO_MAU_SHORT;
      donViHoaLanShort( duLieuAmThanh.lan0, soMau * kNHI_KENH, 1.0f );
      luuAmThanhWAV_short( "ren4_0b.wav", &duLieuAmThanh );
    //  chinhTanSo = 1.0f;
      DieuCheTanSo_short ren5;    // huyên náo
      ren5.giaiThuat = 7;
      duLieuAmThanh.soLuongLan = 1;
      
      ren5.tongAmLuong = 1;
      ren5.phanHoi = 0;
      
      ren5.amLuong.m1 = 376;
      ren5.amLuong.m2 = 103;
      ren5.amLuong.c1 = 336;
      ren5.amLuong.c2 = 108;
      ren5.amLuong.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
      ren5.loaiBaoBi = kBAO_BI__COS;
      ren5.tanSoBaoBi = 3.1415926f*0.6667f;
      
      chinhTanSo = 1.0f;
      ren5.tanSo.m1 = 1564672*chinhTanSo;
      ren5.tanSo.m2 = 704512*chinhTanSo;
      ren5.tanSo.c1 = 5978112*chinhTanSo;
      ren5.tanSo.c2 = 498176*chinhTanSo;
      ren5.tanSo.loaiChinh = kCHINH__LOAI_HUYEN_NAO;
      ren5.tanSo.nhipChinh = kCHINH__NHIP_8_MAU;
      ren5.tanSo.bienDo_m1 = 1656833*chinhTanSo;
      ren5.tanSo.bienDo_m2 = 745729*chinhTanSo;
      ren5.tanSo.bienDo_c1 = 6331393*chinhTanSo;
      ren5.tanSo.bienDo_c2 = 527617*chinhTanSo;
      
            giaiThuat7_short( duLieuAmThanh.lan0, kTAN_SO_MAU_SHORT, soMau, &ren5, 3.0f );
            duLieuAmThanh.soLuongMau = 3.0f*kTAN_SO_MAU_SHORT;
            donViHoaLanShort( duLieuAmThanh.lan0, soMau * kNHI_KENH, 1.0f );
            luuAmThanhWAV_short( "ren5_0.wav", &duLieuAmThanh );
      
      exit(0);

      DieuCheTanSo_short sao0;    // sáo 0
      sao0.giaiThuat = 3;
      duLieuAmThanh.soLuongLan = 1;
      
      sao0.tongAmLuong = 1;
      sao0.phanHoi = 0;
      
      sao0.amLuong.m1 = 264;
      sao0.amLuong.m2 = 344;
      sao0.amLuong.c1 = 184;
      sao0.amLuong.c2 = 96;
      sao0.amLuong.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
      
      chinhTanSo = 1.0f;
      sao0.tanSo.m1 = 1674752*chinhTanSo;
      sao0.tanSo.m2 = 8389122*chinhTanSo;
      sao0.tanSo.c1 = 838912*chinhTanSo;
      sao0.tanSo.c2 = 837376*chinhTanSo;
      sao0.tanSo.loaiChinh = kCHINH__LOAI_KHONG_CHINH;

//      giaiThuat3_short( duLieuAmThanh.lan0, kTAN_SO_MAU_SHORT, soMau, &sao0, 8.0f );
//      duLieuAmThanh.soLuongMau = 8.0f*kTAN_SO_MAU_SHORT;
//      donViHoaLanShort( duLieuAmThanh.lan0, soMau * kNHI_KENH, 1.0f );
//      luuAmThanhWAV_short( "sao0_Y.wav", &duLieuAmThanh );
      
/*      DieuCheTanSo_short sao1;    // sáo 1
      sao1.giaiThuat = 4;
      duLieuAmThanh.soLuongLan = 1;
      
      sao1.tongAmLuong = 1;
      sao1.phanHoi = 0;
      
      sao1.amLuong.m1 = 304;
      sao1.amLuong.m2 = 232;
      sao1.amLuong.c1 = 136;
      sao1.amLuong.c2 = 127;
      sao1.amLuong.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
      char tenTapTin[255];
      
      unsigned chiSoTanSo = 0;
      while( chiSoTanSo < 16 ) {
         sprintf( tenTapTin, "sao1_%c.wav", 'A' + chiSoTanSo );
      chinhTanSo = mangTanSo[chiSoTanSo];
      sao1.tanSo.m1 = 1056896*chinhTanSo;
      sao1.tanSo.m2 = 527296*chinhTanSo;
      sao1.tanSo.c1 = 2109184*chinhTanSo;
      sao1.tanSo.c2 = 1056896*chinhTanSo;
      sao1.tanSo.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
         
      xoaMauInt( duLieuAmThanh.lan0, duLieuAmThanh.soLuongMau << 1 );
      
      giaiThuat4_short( duLieuAmThanh.lan0, kTAN_SO_MAU_SHORT, soMau, &sao1, 8.0f );
      duLieuAmThanh.soLuongMau = 8.0f*kTAN_SO_MAU_SHORT;
      donViHoaLanShort( duLieuAmThanh.lan0, soMau * kNHI_KENH, 1.0f );
      luuAmThanhWAV_short( tenTapTin, &duLieuAmThanh );
         chiSoTanSo++;
      } */
      
      DieuCheTanSo_short ghen3;    // sáo 0
      ghen3.giaiThuat = 3;
      duLieuAmThanh.soLuongLan = 1;
      
      ghen3.tongAmLuong = 1;
      ghen3.phanHoi = 0;
      
      ghen3.amLuong.m1 = 267;
      ghen3.amLuong.m2 = 175;
      ghen3.amLuong.c1 = 170;
      ghen3.amLuong.c2 = 1110;
      ghen3.amLuong.loaiChinh = kCHINH__LOAI_DOC;
      ghen3.amLuong.tanSoChinh = 2.0f;
      ghen3.amLuong.bienDo_m1 = 0;
      ghen3.amLuong.bienDo_m2 = 0;
      ghen3.amLuong.bienDo_c1 = 0;
      ghen3.amLuong.bienDo_c2 = -600;
      
      chinhTanSo = 1.0f;
      ghen3.tanSo.m1 = 559360*chinhTanSo;
      ghen3.tanSo.m2 = 559360*chinhTanSo;
      ghen3.tanSo.c1 = 559360*chinhTanSo;
      ghen3.tanSo.c2 = 559360*chinhTanSo;
      ghen3.tanSo.loaiChinh = kCHINH__LOAI_KHONG_CHINH;
      
      giaiThuat5_short( duLieuAmThanh.lan0, kTAN_SO_MAU_SHORT, soMau, &ghen3, 8.0f );
      duLieuAmThanh.soLuongMau = 8.0f*kTAN_SO_MAU_SHORT;
      donViHoaLanShort( duLieuAmThanh.lan0, soMau * kNHI_KENH, 1.0f );
      luuAmThanhWAV_short( "Ghen3_E.wav", &duLieuAmThanh );
   }

   return 1;
}

void giaiThuat0_short( int *demAm, unsigned int tocDoMau, unsigned int soMau, DieuCheTanSo_short *dan, float thoiGianHat ) {
   
   float thoiGian = 0.0f;   // thời gian tương đối của nốt (giây)
   float nhipThoiGian = 1.0f/tocDoMau; // giây
   unsigned char loaiBaoBi = dan->loaiBaoBi;
   float tanSoBaoBi = dan->tanSoBaoBi;
   
   int m1 = 0;
   int m2 = 0;
   int c1 = 0;
   int c2 = 0;
   
   unsigned int amLuong_m1 = dan->amLuong.m1;
   unsigned int amLuong_m2 = dan->amLuong.m2;
   unsigned int amLuong_c1 = dan->amLuong.c1;
   unsigned int amLuong_c2 = dan->amLuong.c2;
   
   unsigned int tanSo_m1 = dan->tanSo.m1;
   unsigned int tanSo_m2 = dan->tanSo.m2;
   unsigned int tanSo_c1 = dan->tanSo.c1;
   unsigned int tanSo_c2 = dan->tanSo.c2;
   
   unsigned int pha_m1 = 0;
   unsigned int pha_m2 = 0;
   unsigned int pha_c1 = 0;
   unsigned int pha_c2 = 0;
   
   short phaPhanHoi = dan->phanHoi;
   short phanHoi_truoc = 0;
   short phanHoi_hienTai = 0;
 
   unsigned char loaiChinhAmLuong = dan->amLuong.loaiChinh;
   unsigned char loaiChinhTanSo = dan->tanSo.loaiChinh;
 
   // ---- áp dụng giải thuật tính mẫu
   unsigned int chiSoMau = 0;
   unsigned int chiSoMauCuoi = (unsigned int)(thoiGianHat*tocDoMau);
   chiSoMauCuoi <<= 1;   // nhị kênh
   soMau <<= 1; // nhị kênh
   
   while( chiSoMau < chiSoMauCuoi ) {
      // ---- nâng cấp tổng âm lượng
      float tongAm = expf( -4.0*thoiGian );
      
      // ---- tính phản hội - nếu có
      short phanHoi = tinhPhanHoi( &phanHoi_truoc, &phanHoi_hienTai, phaPhanHoi, amLuong_m1, pha_m1 );
      
      c1 = phanHoi_truoc;

      int boNho = 0;
      if( amLuong_c1 < 832 )
         boNho = tinhCM12( pha_c1, amLuong_c1, c1 );
//      printf( "m2: m1 %d m2 %d c1 %d c2 %d pha %d\n", m1, m2, c1, c2, pha_m2 );
//      printf( "c1: m1(boNho) %d m2 %d c1 %d c2 %d  phanHoi_truoc %d  phanHoiHT %d\n", boNho, m2, c1, c2, phanHoi_truoc, phanHoi_hienTai );
      c2 = 0;
      if( amLuong_m2 < 832 )
         c2 = tinhCM12( pha_m2, amLuong_m2, m2 );
      
      int mau = 0;
      if( amLuong_c2 < 832 )
         mau = tinhCM12( pha_c2, amLuong_c2, c2 );
      
//      printf( "  c2: m1(boNho) %d m2 %d c1 %d c2 %d - mau %d\n", boNho, m2, c1, c2, mau );
//      printf( "    pha_m1 %d  pha_m2 %d  pha_c1 %d  pha_c2 %d - mau %d\n\n",  pha_m1, pha_m2, pha_c1, pha_c2, mau );

      
      m2 = boNho;
      
      //      printf( "m1: m1 %d m2 %d c1 %d c2 %d  pha %d - mau %d\n", m1, m2, c1, c2, pha_m1, mau );
      mau *= tongAm;
      if( chiSoMau < soMau ) {
         *demAm = mau;
         demAm++;
         *demAm = mau;
         demAm++;
      }
      
      // ---- nâng cấp âm lượng nếu cần
      if( loaiChinhAmLuong )
         chinhHeSo( &(dan->amLuong), loaiChinhAmLuong, chiSoMau, thoiGian,
                   &amLuong_m1, &amLuong_m2, &amLuong_c1, &amLuong_c2 );
      
      // ---- nâng cấp tần số nếu cần
      if( loaiChinhTanSo )
         chinhHeSo( &(dan->tanSo), loaiChinhTanSo, chiSoMau, thoiGian,
                   &tanSo_m1, &tanSo_m2, &tanSo_c1, &tanSo_c2 );
      
      pha_m1 += tanSo_m1; // rad/giây
      pha_m2 += tanSo_m2; // rad/giây
      pha_c1 += tanSo_c1; // rad/giây
      pha_c2 += tanSo_c2; // rad/giây
      
      chiSoMau += 2;
      thoiGian += nhipThoiGian;
      
//      if( chiSoMau > 10 )
//         exit(0);
   }

}


void giaiThuat1_short( int *demAm, unsigned int tocDoMau, unsigned int soMau, DieuCheTanSo_short *dan, float thoiGianHat ) {

}

void giaiThuat2_short( int *demAm, unsigned int tocDoMau, unsigned int soMau, DieuCheTanSo_short *dan, float thoiGianHat ) {
   
   float thoiGian = 0.0f;   // thời gian tương đối của nốt (giây)
   float nhipThoiGian = 1.0f/tocDoMau; // giây
   unsigned char loaiBaoBi = dan->loaiBaoBi;
   float tanSoBaoBi = dan->tanSoBaoBi;
   
   unsigned int m1 = 0;
   unsigned int m2 = 0;
   unsigned int c1 = 0;
   unsigned int c2 = 0;
   
   unsigned int amLuong_m1 = dan->amLuong.m1;
   unsigned int amLuong_m2 = dan->amLuong.m2;
   unsigned int amLuong_c1 = dan->amLuong.c1;
   unsigned int amLuong_c2 = dan->amLuong.c2;
   
   unsigned int tanSo_m1 = dan->tanSo.m1;
   unsigned int tanSo_m2 = dan->tanSo.m2;
   unsigned int tanSo_c1 = dan->tanSo.c1;
   unsigned int tanSo_c2 = dan->tanSo.c2;
   
   unsigned int pha_m1 = 0;
   unsigned int pha_m2 = 0;
   unsigned int pha_c1 = 0;
   unsigned int pha_c2 = 0;
   
   short phaPhanHoi = dan->phanHoi;
   short phanHoi_truoc = 0;
   short phanHoi_hienTai = 0;
   
   unsigned char loaiChinhAmLuong = dan->amLuong.loaiChinh;
   unsigned char loaiChinhTanSo = dan->tanSo.loaiChinh;

   // ---- áp dụng giải thuật tính mẫu
   unsigned int chiSoMau = 0;
   unsigned int chiSoMauCuoi = (unsigned int)(thoiGianHat*tocDoMau);
   chiSoMauCuoi <<= 1;   // nhị kênh
   soMau <<= 1; // nhị kênh
   
   while( chiSoMau < chiSoMauCuoi ) {
      // ---- nâng cấp tổng âm lượng
      float tongAm = tinhTongAmLuongChoLoaiBaoBi( loaiBaoBi, tanSoBaoBi, thoiGian );
      
      // ---- tính phản hội - nếu có
      short phanHoi = tinhPhanHoi( &phanHoi_truoc, &phanHoi_hienTai, phaPhanHoi, amLuong_m1, pha_m1 );
      
//      printf( "  m1 %d m2 %d c1 %d c2 %d  phanHoi_truoc %d  hienTai %d\n", m1, m2, c1, c2, phanHoi_truoc, phanHoi_hienTai );
      
      c2 = 0;
      if( amLuong_m2 < 832 )
         c2 = tinhCM12( pha_m2, amLuong_m2, m2 ) + phanHoi_truoc;

      short boNho = 0;
      if( amLuong_c1 < 832 )
         boNho = tinhCM12( pha_c1, amLuong_c1, c1 );
      
      short mau = 0;
      if( amLuong_c2 < 832 )
         mau = tinhCM12( pha_c2, amLuong_c2, c2 );
      
      if( mau > 16384 )
         mau = 16384;
      else if( mau < -16384 )
         mau = -16384;
      
//      printf( "  c2: m1(boNho) %d m2 %d c1 %d c2 %d - mau %d\n", boNho, m2, c1, c2, mau );
//      printf( "    pha_m1 %d  pha_m2 %d  pha_c1 %d  pha_c2 %d - mau %d\n\n",  pha_m1, pha_m2, pha_c1, pha_c2, mau );
      
      m2 = boNho;

      if( chiSoMau < soMau ) {
         float mauChinhAmLuong = ((float)mau*tongAm);
         *demAm += (short)mauChinhAmLuong;
         demAm++;
         *demAm += (short)mauChinhAmLuong;
         demAm++;
      }
      
      // ---- nâng cấp âm lượng nếu cần
      if( loaiChinhAmLuong )
         chinhHeSo( &(dan->amLuong), loaiChinhAmLuong, chiSoMau, thoiGian,
                   &amLuong_m1, &amLuong_m2, &amLuong_c1, &amLuong_c2 );
      
      // ---- nâng cấp tần số nếu cần
      if( loaiChinhTanSo )
         chinhHeSo( &(dan->tanSo), loaiChinhTanSo, chiSoMau, thoiGian,
                   &tanSo_m1, &tanSo_m2, &tanSo_c1, &tanSo_c2 );
      
      pha_m1 += tanSo_m1;
      pha_m2 += tanSo_m2;
      pha_c1 += tanSo_c1;
      pha_c2 += tanSo_c2;
      
      chiSoMau += 2;
      thoiGian += nhipThoiGian;
      
//      if( chiSoMau > 100 )
//         exit(0);
   }
   
}

/*
  m1:  env_m1•sin( pha_m1 + m1) --> c1  
  m2:  env_m2•sin( pha_m2 + m2) --> c2
  
  c1:  env_c1•sin( pha_c1 + c1) --> ra
  c2:  env_c2•sin( pha_c2 + c2) --> += ra
*/

void giaiThuat3_short( int *demAm, unsigned int tocDoMau, unsigned int soMau, DieuCheTanSo_short *dan, float thoiGianHat ) {
   
   float thoiGian = 0.0f;   // thời gian tương đối của nốt (giây)
   float nhipThoiGian = 1.0f/tocDoMau; // giây
   unsigned char loaiBaoBi = dan->loaiBaoBi;
   float tanSoBaoBi = dan->tanSoBaoBi;
   
   int m1 = 0;
   int m2 = 0;
   int c1 = 0;
   int c2 = 0;
   
   unsigned int amLuong_m1 = dan->amLuong.m1;
   unsigned int amLuong_m2 = dan->amLuong.m2;
   unsigned int amLuong_c1 = dan->amLuong.c1;
   unsigned int amLuong_c2 = dan->amLuong.c2;
   
   unsigned int tanSo_m1 = dan->tanSo.m1;
   unsigned int tanSo_m2 = dan->tanSo.m2;
   unsigned int tanSo_c1 = dan->tanSo.c1;
   unsigned int tanSo_c2 = dan->tanSo.c2;
   
   unsigned int pha_m1 = 0;
   unsigned int pha_m2 = 0;
   unsigned int pha_c1 = 0;
   unsigned int pha_c2 = 0;
   
   short phaPhanHoi = dan->phanHoi;
   short phanHoi_truoc = 0;
   short phanHoi_hienTai = 0;
   
   unsigned char loaiChinhAmLuong = dan->amLuong.loaiChinh;
   unsigned char loaiChinhTanSo = dan->tanSo.loaiChinh;
   
   int boNho = 0;

   // ---- áp dụng giải thuật tính mẫu
   unsigned int chiSoMau = 0;
   unsigned int chiSoMauCuoi = (unsigned int)(thoiGianHat*tocDoMau);
   chiSoMauCuoi <<= 1;   // nhị kênh
   soMau <<= 1; // nhị kênh
   
   while( chiSoMau < chiSoMauCuoi ) {
      // ---- nâng cấp tổng âm lượng
      float tongAm = tinhTongAmLuongChoLoaiBaoBi( loaiBaoBi, tanSoBaoBi, thoiGian );
      
      // ---- tính phản hội - nếu có
      short phanHoi = tinhPhanHoi( &phanHoi_truoc, &phanHoi_hienTai, phaPhanHoi, amLuong_m1, pha_m1 );
      
      c2 = boNho;

//      printf( "  m1(boNho) %d m2 %d c1 %d c2 %d  phanHoi_truoc %d  hienTai %d\n", boNho, m2, c1, c2, phanHoi_truoc, phanHoi_hienTai );
      
      c1 = phanHoi_truoc;

      // ---- chứa lại
      boNho = 0;
      if( amLuong_c1 < 832 )
         boNho = tinhCM12( pha_c1, amLuong_c1, c1 );

      if( amLuong_m2 < 832 )
         c2 += tinhCM12( pha_m2, amLuong_m2, m2 );

      int mau = 0;
      
      if( amLuong_c2 < 832 )
         mau += tinhCM12( pha_c2, amLuong_c2, c2 );
      mau *= tongAm;
 //     printf( "mau: %d\n", mau );
      
      if( chiSoMau < soMau ) {
         *demAm = mau;
         demAm++;
         *demAm = mau;
         demAm++;
      }
      
//      printf( "  c2: m1(boNho) %d m2 %d c1 %d c2 %d - mau %d\n", boNho, m2, c1, c2, mau );
//      printf( "    pha_m1 %d  pha_m2 %d  pha_c1 %d  pha_c2 %d - mau %d\n\n",  pha_m1, pha_m2, pha_c1, pha_c2, mau );
      
      // ---- nâng cấp âm lượng nếu cần
      if( loaiChinhAmLuong )
         chinhHeSo( &(dan->amLuong), loaiChinhAmLuong, chiSoMau, thoiGian,
                   &amLuong_m1, &amLuong_m2, &amLuong_c1, &amLuong_c2 );
      
      // ---- nâng cấp tần số nếu cần
      if( loaiChinhTanSo )
         chinhHeSo( &(dan->tanSo), loaiChinhTanSo, chiSoMau, thoiGian,
                   &tanSo_m1, &tanSo_m2, &tanSo_c1, &tanSo_c2 );
      
      pha_m1 += tanSo_m1; // rad/giây
      pha_m2 += tanSo_m2; // rad/giây
      pha_c1 += tanSo_c1; // rad/giây
      pha_c2 += tanSo_c2; // rad/giây
      
      chiSoMau += 2;
      thoiGian += nhipThoiGian;
      
//     if( chiSoMau == 10 )
//         exit(0);
   }
   
}

void giaiThuat4_short( int *demAm, unsigned int tocDoMau, unsigned int soMau, DieuCheTanSo_short *dan, float thoiGianHat ) {
   
   float thoiGian = 0.0f;   // thời gian tương đối của nốt (giây)
   float nhipThoiGian = 1.0f/tocDoMau; // giây
   unsigned char loaiBaoBi = dan->loaiBaoBi;
   float tanSoBaoBi = dan->tanSoBaoBi;

   unsigned int m1 = 0;
   unsigned int m2 = 0;
   unsigned int c1 = 0;
   unsigned int c2 = 0;
   
   unsigned int amLuong_m1 = dan->amLuong.m1;
   unsigned int amLuong_m2 = dan->amLuong.m2;
   unsigned int amLuong_c1 = dan->amLuong.c1;
   unsigned int amLuong_c2 = dan->amLuong.c2;
   
   unsigned int tanSo_m1 = dan->tanSo.m1;
   unsigned int tanSo_m2 = dan->tanSo.m2;
   unsigned int tanSo_c1 = dan->tanSo.c1;
   unsigned int tanSo_c2 = dan->tanSo.c2;
   
   unsigned int pha_m1 = 0;
   unsigned int pha_m2 = 0;
   unsigned int pha_c1 = 0;
   unsigned int pha_c2 = 0;
   
   short phaPhanHoi = dan->phanHoi;
   short phanHoi_truoc = 0;
   short phanHoi_hienTai = 0;
   
   unsigned char loaiChinhAmLuong = dan->amLuong.loaiChinh;
   unsigned char loaiChinhTanSo = dan->tanSo.loaiChinh;
   
   // ---- áp dụng giải thuật tính mẫu
   unsigned int chiSoMau = 0;
   unsigned int chiSoMauCuoi = (unsigned int)(thoiGianHat*tocDoMau);
   chiSoMauCuoi <<= 1;   // nhị kênh
   soMau <<= 1; // nhị kênh

   while( chiSoMau < chiSoMauCuoi ) {
      // ---- nâng cấp tổng âm lượng 
      float tongAm = tinhTongAmLuongChoLoaiBaoBi( loaiBaoBi, tanSoBaoBi, thoiGian );

      // ---- tính phản hội - nếu có
      short phanHoi = tinhPhanHoi( &phanHoi_truoc, &phanHoi_hienTai, phaPhanHoi, amLuong_m1, pha_m1 );
      
      c1 = phanHoi_truoc;

//      printf( "--m1 %d m2 %d c1 %d c2 %d phanHoiHT %d truoc %d\n", m1, m2, c1, c2, phanHoi_hienTai, phanHoi_truoc );
      if( amLuong_m2 < 832 )
      c2 = tinhCM12( pha_m2, amLuong_m2, m2 );
//      printf( "  m2: m1 %d m2 %d c1 %d c2 %d\n", m1, m2, c1, c2 );
   
      int mau = 0;
      if( amLuong_c1 < 832 )
         mau = tinhCM12( pha_c1, amLuong_c1, c1 );

      if( amLuong_c2 < 832 )
         mau += tinhCM12( pha_c2, amLuong_c2, c2 );

      
//      printf( "  c2: m1 %d m2 %d c1 %d c2 %d\n", m1, m2, c1, c2 );
//      printf( "    pha_m1 %d  pha_m2 %d  pha_c1 %d  pha_c2 %d --> mau %d\n",  pha_m1, pha_m2, pha_c1, pha_c2, mau );
//      printf( "    amLuong_m1 %d  amLuong_m2 %d  amLuong_c1 %d  amLuong_c2 %d\n\n",  amLuong_m1, amLuong_m2, amLuong_c1, amLuong_c2 );

      if( chiSoMau < soMau ) {
         float mauChinhAmLuong = ((float)mau*tongAm);
         *demAm += (short)mauChinhAmLuong;
         demAm++;
         *demAm += (short)mauChinhAmLuong;
         demAm++;
      }
      
      // ---- nâng cấp âm lượng nếu cần
      if( loaiChinhAmLuong )
         chinhHeSo( &(dan->amLuong), loaiChinhAmLuong, chiSoMau, thoiGian,
                   &amLuong_m1, &amLuong_m2, &amLuong_c1, &amLuong_c2 );
      
      // ---- nâng cấp tần số nếu cần      
      if( loaiChinhTanSo )
         chinhHeSo( &(dan->tanSo), loaiChinhTanSo, chiSoMau, thoiGian,
                   &tanSo_m1, &tanSo_m2, &tanSo_c1, &tanSo_c2 );

      pha_m1 += tanSo_m1;
      pha_m2 += tanSo_m2;
      pha_c1 += tanSo_c1;
      pha_c2 += tanSo_c2;

      chiSoMau += 2;
      thoiGian += nhipThoiGian;
      
//      if( chiSoMau == 10 )
//         exit(0);
   }

}

void giaiThuat5_short( int *demAm, unsigned int tocDoMau, unsigned int soMau, DieuCheTanSo_short *dan, float thoiGianHat ) {
   
   float thoiGian = 0.0f;   // thời gian tương đối của nốt (giây)
   float nhipThoiGian = 1.0f/tocDoMau; // giây
   unsigned char loaiBaoBi = dan->loaiBaoBi;
   float tanSoBaoBi = dan->tanSoBaoBi;
   
   unsigned int m1 = 0;
   unsigned int m2 = 0;
   unsigned int c1 = 0;
   unsigned int c2 = 0;
   
   unsigned int amLuong_m1 = dan->amLuong.m1;
   unsigned int amLuong_m2 = dan->amLuong.m2;
   unsigned int amLuong_c1 = dan->amLuong.c1;
   unsigned int amLuong_c2 = dan->amLuong.c2;
   
   unsigned int tanSo_m1 = dan->tanSo.m1;
   unsigned int tanSo_m2 = dan->tanSo.m2;
   unsigned int tanSo_c1 = dan->tanSo.c1;
   unsigned int tanSo_c2 = dan->tanSo.c2;
   
   unsigned int pha_m1 = 0;
   unsigned int pha_m2 = 0;
   unsigned int pha_c1 = 0;
   unsigned int pha_c2 = 0;
   
   short phaPhanHoi = dan->phanHoi;
   short phanHoi_truoc = 0;
   short phanHoi_hienTai = 0;
   
   unsigned char loaiChinhAmLuong = dan->amLuong.loaiChinh;
   unsigned char loaiChinhTanSo = dan->tanSo.loaiChinh;
   
   // ---- áp dụng giải thuật tính mẫu
   unsigned int chiSoMau = 0;
   unsigned int chiSoMauCuoi = (unsigned int)(thoiGianHat*tocDoMau);
   chiSoMauCuoi <<= 1;   // nhị kênh
   soMau <<= 1; // nhị kênh
   
   int boNho = 0.0f;
   
   while( chiSoMau < chiSoMauCuoi ) {
      // ---- nâng cấp tổng âm lượng
      float tongAm = tinhTongAmLuongChoLoaiBaoBi( loaiBaoBi, tanSoBaoBi, thoiGian );

      // ---- tính phản hội - nếu có
      short phanHoi = tinhPhanHoi( &phanHoi_truoc, &phanHoi_hienTai, phaPhanHoi, amLuong_m1, pha_m1 );
      
      boNho = c1 = c2 = phanHoi_hienTai;

//      printf( "--m1(boNho) %d m2 %d c1 %d c2 %d phanHoiHT %d truoc %d\n", boNho, m2, c1, c2, phanHoi_hienTai, phanHoi_truoc );

      int mau = 0;
      if( amLuong_m2 < 832 )
          mau = tinhCM12( pha_m2, amLuong_m2, m2 );
//      printf( "  m2: mau %d\n", mau );
      if( amLuong_c1 < 832 )
         mau += tinhCM12( pha_c1, amLuong_c1, c1 );
//      printf( "  c1: mau %d\n", mau );
      if( amLuong_c2 < 832 )
         mau += tinhCM12( pha_c2, amLuong_c2, c2 );
//      printf( "  c2: mau %d\n", mau );
      mau *= tongAm;
      
//      printf( "  c2: m1(boNho) %d m2 %d c1 %d c2 %d\n", boNho, m2, c1, c2 );
//      printf( "    pha_m1 %d  pha_m2 %d  pha_c1 %d  pha_c2 %d --> mau %d\n",  pha_m1, pha_m2, pha_c1, pha_c2, mau );
//      printf( "    amLuong_m1 %d  amLuong_m2 %d  amLuong_c1 %d  amLuong_c2 %d\n\n",  amLuong_m1, amLuong_m2, amLuong_c1, amLuong_c2 );
      m2 = boNho;
      
      if( chiSoMau < soMau ) {
         *demAm = mau;
         demAm++;
         *demAm = mau;
         demAm++;
      }
      
      // ---- nâng cấp âm lượng nếu cần
      if( loaiChinhAmLuong )
         chinhHeSo( &(dan->amLuong), loaiChinhAmLuong, chiSoMau, thoiGian,
                   &amLuong_m1, &amLuong_m2, &amLuong_c1, &amLuong_c2 );
      
      // ---- nâng cấp tần số nếu cần
      if( loaiChinhTanSo )
         chinhHeSo( &(dan->tanSo), loaiChinhTanSo, chiSoMau, thoiGian,
                   &tanSo_m1, &tanSo_m2, &tanSo_c1, &tanSo_c2 );
 
      pha_m1 += tanSo_m1; // rad/giây
      pha_m2 += tanSo_m2; // rad/giây
      pha_c1 += tanSo_c1; // rad/giây
      pha_c2 += tanSo_c2; // rad/giây
      
      chiSoMau += 2;
      thoiGian += nhipThoiGian;
      
//      if( chiSoMau == 10 )
//         exit(0);
   }
   
}

/*
void giaiThuat6_short( int *demAm, unsigned int tocDoMau, unsigned int soMau, DieuCheTanSo_short *dan, float thoiGianHat ) {
   
   float thoiGian = 0.0f;   // thời gian tương đối của nốt (giây)
   float nhipThoiGian = 1.0f/tocDoMau; // giây
    unsigned char loaiBaoBi = dan->loaiBaoBi;
    float tanSoBaoBi = dan->tanSoBaoBi;
 
   unsigned int m1 = 0;
   unsigned int m2 = 0;
   unsigned int c1 = 0;
   unsigned int c2 = 0;
 
   unsigned int amLuong_m1 = dan->amLuong.m1;
   unsigned int amLuong_m2 = dan->amLuong.m2;
   unsigned int amLuong_c1 = dan->amLuong.c1;
   unsigned int amLuong_c2 = dan->amLuong.c2;
 
   unsigned int tanSo_m1 = dan->tanSo.m1;
   unsigned int tanSo_m2 = dan->tanSo.m2;
   unsigned int tanSo_c1 = dan->tanSo.c1;
   unsigned int tanSo_c2 = dan->tanSo.c2;
 
   unsigned int pha_m1 = 0;
   unsigned int pha_m2 = 0;
   unsigned int pha_c1 = 0;
   unsigned int pha_c2 = 0;
 
   short phaPhanHoi = dan->phanHoi;
   short phanHoi_truoc = 0;
   short phanHoi_hienTai = 0;
 
   unsigned char chinhTanSoLoai = dan->chinhTanSoLoai;
   unsigned short chinhTanSoNhip = dan->chinhTanSoNhip;

   // ---- áp dụng giải thuật tính mẫu
   unsigned int chiSoMau = 0;
   unsigned int chiSoMauCuoi = (unsigned int)(thoiGianHat*tocDoMau);
   chiSoMauCuoi <<= 1;   // nhị kênh
   soMau <<= 1; // nhị kênh
   
   int boNho = 0.0f;
   
   while( chiSoMau < chiSoMauCuoi ) {
      // ---- nâng cấp tổng âm lượng
      float tongAm = tinhTongAmLuongChoLoaiBaoBi( loaiBaoBi, tanSoBaoBi, thoiGian );
      
      // ---- tính phản hội - nếu có
      short phanHoi = 0;
      if( phaPhanHoi != 0 ) {
         phanHoi = phanHoi_truoc + phanHoi_hienTai;
         //               printf( "%5.3f (s)  phanHoi %d = phanHoi_tr %d + phanHoi_ht %d\n", thoiGian, phanHoi, phanHoi_truoc, phanHoi_hienTai );
         phanHoi_truoc = phanHoi_hienTai;
         phanHoi_hienTai = 0;
         if( amLuong_m1 < 832 )
            phanHoi_hienTai = tinhM1( pha_m1, amLuong_m1, phanHoi << phaPhanHoi );
      }
      else {
         phanHoi_truoc = phanHoi_hienTai;
         phanHoi_hienTai = 0;
         if( amLuong_m1 < 832 )
            phanHoi_hienTai = tinhM1( pha_m1, amLuong_m1, 0 );
      }

      c1 = phanHoi_truoc;

      int mau = 0;
      if( amLuong_m2 < 832 )
         mau = tinhCM12( pha_m2, amLuong_m2, m2 );

      if( amLuong_c1 < 832 )
          mau += tinhCM12( pha_c1, amLuong_c1, c1 );
 
      if( amLuong_c2 < 832 )
          mau += tinhCM12( pha_c2, amLuong_c2, c2 );

      mau *= tongAm;
      
//            printf( "m1: m1(boNho) %8.5f m2 %8.5f c1 %8.5f c2 %8.5f  pha %5.3f - mau %5.3f\n", boNho, m2, c1, c2, pha_m1, mau );
      m2 = boNho;
      
      if( chiSoMau < soMau ) {
         *demAm = mau;
         demAm++;
         *demAm = mau;
         demAm++;
      }

 // ---- nâng cấp âm lượng nếu cần
 if( loaiChinhAmLuong )
 chinhHeSo( &(dan->amLuong), loaiChinhAmLuong, chiSoMau, thoiGian,
 &amLuong_m1, &amLuong_m2, &amLuong_c1, &amLuong_c2 );
 
 // ---- nâng cấp tần số nếu cần
 if( loaiChinhTanSo )
 chinhHeSo( &(dan->tanSo), loaiChinhTanSo, chiSoMau, thoiGian,
   &tanSo_m1, &tanSo_m2, &tanSo_c1, &tanSo_c2 );
 
      pha_m1 += tanSo_m1; // rad/giây
      pha_m2 += tanSo_m2; // rad/giây
      pha_c1 += tanSo_c1; // rad/giây
      pha_c2 += tanSo_c2; // rad/giây
      
      chiSoMau += 2;
      thoiGian += nhipThoiGian;
      
//      if( chiSoMau > 10 )
//         exit(0);
   }
   
}
*/

void giaiThuat7_short( int *demAm, unsigned int tocDoMau, unsigned int soMau, DieuCheTanSo_short *dan, float thoiGianHat ) {
   
   float thoiGian = 0.0f;   // thời gian tương đối của nốt (giây)
   float nhipThoiGian = 1.0f/tocDoMau; // giây
   unsigned char loaiBaoBi = dan->loaiBaoBi;
   float tanSoBaoBi = dan->tanSoBaoBi;
   
   unsigned int m1 = 0;
   unsigned int m2 = 0;
   unsigned int c1 = 0;
   unsigned int c2 = 0;
   
   unsigned int amLuong_m1 = dan->amLuong.m1;
   unsigned int amLuong_m2 = dan->amLuong.m2;
   unsigned int amLuong_c1 = dan->amLuong.c1;
   unsigned int amLuong_c2 = dan->amLuong.c2;
   
   unsigned int tanSo_m1 = dan->tanSo.m1;
   unsigned int tanSo_m2 = dan->tanSo.m2;
   unsigned int tanSo_c1 = dan->tanSo.c1;
   unsigned int tanSo_c2 = dan->tanSo.c2;
   
   unsigned int pha_m1 = 0;
   unsigned int pha_m2 = 0;
   unsigned int pha_c1 = 0;
   unsigned int pha_c2 = 0;
   
   short phaPhanHoi = dan->phanHoi;
   short phanHoi_truoc = 0;
   short phanHoi_hienTai = 0;
   
   unsigned char loaiChinhAmLuong = dan->amLuong.loaiChinh;
   unsigned char loaiChinhTanSo = dan->tanSo.loaiChinh;
   
   // ---- áp dụng giải thuật tính mẫu
   unsigned int chiSoMau = 0;
   unsigned int chiSoMauCuoi = (unsigned int)(thoiGianHat*tocDoMau);
   chiSoMauCuoi <<= 1;   // nhị kênh
   soMau <<= 1; // nhị kênh
   
   while( chiSoMau < chiSoMauCuoi ) {
      // ---- nâng cấp tổng âm lượng
      float tongAm = tinhTongAmLuongChoLoaiBaoBi( loaiBaoBi, tanSoBaoBi, thoiGian );
      
      // ---- tính phản hội - nếu có
      short phanHoi = tinhPhanHoi( &phanHoi_truoc, &phanHoi_hienTai, phaPhanHoi, amLuong_m1, pha_m1 );
 
      int mau = phanHoi_hienTai;
//      printf( "  m1: mau %d  phanHoiHT %d truoc %d\n", mau, phanHoi_hienTai, phanHoi_truoc );
      if( amLuong_m2 < 832 )
         mau += tinhCM12( pha_m2, amLuong_m2, 0 );
//      printf( "  m2: mau %d\n", mau );
      if( amLuong_c1 < 832 )
         mau += tinhCM12( pha_c1, amLuong_c1, 0 );
//      printf( "  c1: mau %d\n", mau );
      if( amLuong_c2 < 832 )
         mau += tinhCM12( pha_c2, amLuong_c2, 0 );
//      printf( "  c2: mau %d\n", mau );
      mau *= tongAm;
      
      if( chiSoMau < soMau ) {
         *demAm = mau;
         demAm++;
         *demAm = mau;
         demAm++;
      }


//      printf( "    pha_m1 %d  pha_m2 %d  pha_c1 %d  pha_c2 %d --> mau %d\n",  pha_m1, pha_m2, pha_c1, pha_c2, mau );
//      printf( "    amLuong_m1 %d  amLuong_m2 %d  amLuong_c1 %d  amLuong_c2 %d\n\n",  amLuong_m1, amLuong_m2, amLuong_c1, amLuong_c2 );

      // ---- nâng cấp âm lượng nếu cần
      if( loaiChinhAmLuong )
         chinhHeSo( &(dan->amLuong), loaiChinhAmLuong, chiSoMau, thoiGian,
                   &amLuong_m1, &amLuong_m2, &amLuong_c1, &amLuong_c2 );
      
      // ---- nâng cấp tần số nếu cần
      if( loaiChinhTanSo )
         chinhHeSo( &(dan->tanSo), loaiChinhTanSo, chiSoMau, thoiGian,
                   &tanSo_m1, &tanSo_m2, &tanSo_c1, &tanSo_c2 );

      pha_m1 += tanSo_m1; // rad/giây
      pha_m2 += tanSo_m2; // rad/giây
      pha_c1 += tanSo_c1; // rad/giây
      pha_c2 += tanSo_c2; // rad/giây

      chiSoMau += 2;
      thoiGian += nhipThoiGian;
      
//      if( chiSoMau == 10 )
//         exit(0);
   }
}

#pragma mark ---- Nâng Cấp Hệ số
void chinhHeSo( HeSoBo_short *bo, unsigned char loaiChinh, unsigned int soMau, float thoiGian,
           unsigned int *giaTri_m1, unsigned int *giaTri_m2, unsigned int *giaTri_c1, unsigned int *giaTri_c2 ) {
   
   float bienDo = 0.0f;

   
   float giaTriTinh = thoiGian*bo->tanSoChinh;

   if( loaiChinh == kCHINH__LOAI_DOC ) {
      
      bienDo = doc( giaTriTinh, 0.0f, 1.0f, 0.0f, 1.0f );

   }
   else if( loaiChinh == kCHINH__LOAI_TAM_GIAC ) {
      
      bienDo = songTamGiacf( giaTriTinh );

   }
   else if( loaiChinh == kCHINH__LOAI_HUYEN_NAO ) {
      unsigned short nhipChinh = bo->nhipChinh;

      // ---- xem tới thời gian đổi chưa
      if( (soMau & nhipChinh) == 0 ) {
         
         bienDo = (float)(rand() & 0x3ff)/512.0f - 1.0f;
      
      }
      else  // không có gì để làm
         return;
   }
   else if( loaiChinh == kCHINH__LOAI_PARABOL ) {
      
      bienDo = dayParabol( giaTriTinh, 0.0f, 1.0f, 0.0f, 1.0f );

   }
   else  // không có gì để làm
      return;
   
   // ---- nâng cấp
   *giaTri_m1 = bo->m1 + bo->bienDo_m1*bienDo;
   *giaTri_m2 = bo->m2 + bo->bienDo_m2*bienDo;
   *giaTri_c1 = bo->c1 + bo->bienDo_c1*bienDo;
   *giaTri_c2 = bo->c2 + bo->bienDo_c2*bienDo;
}

void phaTronHetLanVaoLan0( DuLieuAmThanh_short *duLieuAmThanh ) {
   
   int *mangMau = duLieuAmThanh->mangMau;
   unsigned int soLuongMau = duLieuAmThanh->soLuongMau;
   unsigned int dichLan = soLuongMau * kNHI_KENH;
   unsigned int soLuongLan = duLieuAmThanh->soLuongLan;
   
   // ==== cộng lên hết làn
   unsigned char soLan = 1;  // bỏ qua làn đi vì cộng vào nó
   unsigned int diaChiLan = dichLan;
   while( soLan < soLuongLan ) {
      unsigned int soMau = 0;
      while( soMau < soLuongMau ) {
         // ---- cộng lên hết mẫu
         
         mangMau[soMau] += mangMau[diaChiLan+soMau];
         mangMau[soMau+1] += mangMau[diaChiLan+soMau+1];
         
         soMau += 2;  // nhị kênh
      }
      
      diaChiLan += dichLan;
      soLan++;
   }

   // ==== Đơn vị hóa làn 0
   donViHoaLanShort( mangMau, soLuongMau*kNHI_KENH, 0.8f );
}

void donViHoaLanShort( int *lan,  unsigned int soLuongMau, float muc ) {
   
   // ---- tìm độ lớn cực đại
   unsigned int soMau = 0;
   int doLonCucDai = 0;
   while( soMau < soLuongMau ) {
      int mau = lan[soMau];
      // ---- giá trị tuyệt đối
      if( mau < 0 )
         mau = -mau;

      if( mau > doLonCucDai )
         doLonCucDai = mau;

      soMau++;
   }

   // ---- đơn vị hóa phạm vi -32768   +32767
   float doLonCucDai_float = (float)doLonCucDai;
   if( doLonCucDai > 0 ) {
      soMau = 0;
      while( soMau < soLuongMau ) {
         float mau = (float)lan[soMau];
         lan[soMau] = (short)(mau*32767.0f*muc / doLonCucDai);
         soMau++;
      }
   }
}


void chepVamDem_short( DuLieuAmThanh_short *demPhatAm, DuLieuAmThanh_short *demDiaPhuong, float thoiChepVao ) {
   
   unsigned int soMau = 0;
   unsigned int soLuongMauChep = demDiaPhuong->soLuongMau*2;
   unsigned int diacChiChepVao = thoiChepVao*demPhatAm->tocDoMau*2;
   
   int *demDich = demPhatAm->mangMau + diacChiChepVao;
   int *demNguon = demDiaPhuong->mangMau;

   while( soMau < soLuongMauChep ) {
      *demDich = *demNguon;
      demDich++;
      demNguon++;
      soMau++;
   }
   
}

#pragma mark ---- Chuẩn Bị Bảng Mẫu (cho tính nhanh)  
void chuanBiBangSin() {

   float log2 = logf( 2.0f );
   unsigned short chiSo = 0;
   while(chiSo < 2048 ) {
      float sin = sinf( (chiSo + 1)*kPI/1024.0f );
      if( sin > 0.0f ) {
         bangSin[chiSo>>1] = roundf( 512.0f*log(1.0f/sin)/log2 );
         if( (bangSin[chiSo>>1] & 1) )
            bangSin[chiSo>>1]++;
      }
      else {
         bangSin[chiSo>>1] = roundf( 512.0f*log(-1.0f/sin)/log2 );
         if( !(bangSin[chiSo>>1] & 1) )
            bangSin[chiSo>>1]++;
      }
      chiSo += 2;
   }
}

void chuanBiBangAmLuong() {
   
   unsigned short chiSo = 0;
   while( chiSo < 256 ) {
      float amLuong = 8192.0f*powf( 0.5f, (chiSo+1)/256.0f );

      unsigned short chiSoTrongBang = chiSo<<1;
      unsigned char buoc = 0;
      while( buoc < 13 ) {
         bangAmLuong[chiSoTrongBang] = floorf( amLuong );
         bangAmLuong[chiSoTrongBang+1] = -bangAmLuong[chiSoTrongBang];
         amLuong *= 0.5f;
         chiSoTrongBang += 512;
         buoc++;
      }
      chiSo++;
   }
}


#pragma mark ==== BAO ÂM (HÌNH DẠNG) CHO TỔNG ÂM LƯỢNG
float tinhTongAmLuongChoLoaiBaoBi( unsigned char loaiBaoBi, float tanSo, float thoiGian ) {
   
   float tongAm = 1.0f;
   if( loaiBaoBi == kBAO_BI__LUY_THUA )
      tongAm = expf( -tanSo*thoiGian );
   else if( kBAO_BI__COS )
      tongAm = 0.5f - 0.5f*cosf( tanSo*thoiGian );
   
   return tongAm;
}

#pragma mark ---- Tính Phản Hồi
short tinhPhanHoi( short *phanHoi_truoc, short *phanHoi_hienTai, short phaPhanHoi, unsigned int amLuong_m1, unsigned int pha_m1 ) {
   
   short phanHoi = 0;
   
   if( phaPhanHoi != 0 ) {
      phanHoi = *phanHoi_truoc + *phanHoi_hienTai;
      //               printf( "%5.3f (s)  phanHoi %d = phanHoi_tr %d + phanHoi_ht %d\n", thoiGian, phanHoi, *phanHoi_truoc, *phanHoi_hienTai );
      // ---- giữ phản hồi hiện tại
      *phanHoi_truoc = *phanHoi_hienTai;
      
      // ---- tính phản hồi mới hiện tại
      *phanHoi_hienTai = 0;
      if( amLuong_m1 < 832 )
         *phanHoi_hienTai = tinhM1( pha_m1, amLuong_m1, phanHoi << phaPhanHoi );
   }
   else {
      *phanHoi_truoc = *phanHoi_hienTai;
      *phanHoi_hienTai = 0;
      if( amLuong_m1 < 832 )
         *phanHoi_hienTai = tinhM1( pha_m1, amLuong_m1, 0 );
   }
   
   return phanHoi;
}

#pragma mark ---- Tính M1
short tinhM1( int pha, unsigned short amLuong, short phanHoi ) {
   
   unsigned short chiSoBangSin = ((pha & 0xffff0000) + phanHoi) >> 16;
   chiSoBangSin &= 0x3ff;
   unsigned short chiSoBangAmLuong = (amLuong << 3) + bangSin[chiSoBangSin];
//      printf( " tinhM1: pha %d  amLuong %d  phanHoi %d chiSoSin %d sin[] %d  chiSoAmLuong %d  bangAmLuong[] %d\n",
//             pha, amLuong, phanHoi, chiSoBangSin, bangSin[chiSoBangSin],
//             chiSoBangAmLuong, bangAmLuong[chiSoBangAmLuong] );

   if( chiSoBangAmLuong < kBE_DAI_BAN_AM_LUONG )
      return bangAmLuong[chiSoBangAmLuong];
   else
      return 0;
}

short tinhCM12( int pha, unsigned short amLuong, short chinh ) {
   
   unsigned short chiSoBangSin = ((pha & 0xffff0000) + (chinh << 15)) >> 16;
   chiSoBangSin &= 0x3ff;
   unsigned short chiSoBangAmLuong = (amLuong << 3) + bangSin[chiSoBangSin];
//   printf( " tinhCM12 pha %d  amLuong %d  chinh %d chiSoSin %d sin[] %d  chiSoAmLuong %d  bangAmLuong[] %d\n",
//          pha, amLuong, chinh, chiSoBangSin, bangSin[chiSoBangSin],
//          chiSoBangAmLuong, bangAmLuong[chiSoBangAmLuong] );

   if( chiSoBangAmLuong < kBE_DAI_BAN_AM_LUONG )
      return bangAmLuong[chiSoBangAmLuong];
   else
      return 0;
}

#pragma mark ---- Xóa
void xoaMauInt( int *mangMau, unsigned int soLuongMau ) {
   
   unsigned int chiSoMau = 0;
   while( chiSoMau < soLuongMau ) {
      mangMau[chiSoMau] = 0;
      chiSoMau++;
   }
}


#pragma mark ---- Hình Dạng
float doc( float viTriTinh, float viTriDau, float viTriCuoi, float doCaoDau, float doCaoCuoi ) {
   
   if( viTriTinh < viTriDau )
      return doCaoDau;
   else if( viTriTinh > viTriCuoi )
      return doCaoCuoi;
   else {
      float beDaiDoc = viTriCuoi - viTriDau;
      float viTriTuongDoi = (viTriTinh - viTriDau)/beDaiDoc;
      return (doCaoCuoi - doCaoDau)*viTriTuongDoi + doCaoDau;
   }
}

// bắt đầu từ đấy/đỉnh của parabol  đ(t) = m0*(t - viTriDau)^2 + doCaoDau
// doCaoDau = m0*(0)^2 + b
// doCaoCuoi = m0*(chenhLechViTri)^2 + b
float dayParabol( float viTriTinh, float viTriDau, float viTriCuoi, float doCaoDau, float doCaoCuoi ) {
   
   
   if( viTriTinh < viTriDau )
      return doCaoDau;
   else if( viTriTinh > viTriCuoi )
      return doCaoCuoi;
   else {
      float viTriTuongDoi = viTriTinh - viTriDau;
      float chenhLechViTri = viTriCuoi - viTriDau;
      float m0 = 0.0f;
      if( chenhLechViTri != 0.0f )
         m0 = (doCaoCuoi - doCaoDau)/(chenhLechViTri*chenhLechViTri);
      else
         m0 = (doCaoCuoi - doCaoDau);
      
      return m0*viTriTuongDoi*viTriTuongDoi + doCaoDau;
   }
   
}


float songTamGiacf( float giaTri ) {
   
   // ---- hạn chế phạm vi
   float so = fmodf( giaTri, 1.0f );

   if( so < 0.25f )
      return (4.0f*so);
   else if( so < 0.75f )
      return (2.0f - 4.0f*so);  // 0 = -4(0,5) + b = -2 + b
   else
      return -4.0f + 4.0f*so;
}


