typedef unsigned char   undefined;

typedef unsigned char    bool;
typedef unsigned char    byte;
typedef unsigned int    dword;
typedef unsigned long    qword;
typedef unsigned int    uint;
typedef unsigned long    ulong;
typedef unsigned char    undefined1;
typedef unsigned short    undefined2;
typedef unsigned int    undefined4;
typedef unsigned long    undefined8;
typedef unsigned short    ushort;
typedef unsigned short    word;
#define unkbyte9   unsigned long long
#define unkbyte10   unsigned long long
#define unkbyte11   unsigned long long
#define unkbyte12   unsigned long long
#define unkbyte13   unsigned long long
#define unkbyte14   unsigned long long
#define unkbyte15   unsigned long long
#define unkbyte16   unsigned long long

#define unkuint9   unsigned long long
#define unkuint10   unsigned long long
#define unkuint11   unsigned long long
#define unkuint12   unsigned long long
#define unkuint13   unsigned long long
#define unkuint14   unsigned long long
#define unkuint15   unsigned long long
#define unkuint16   unsigned long long

#define unkint9   long long
#define unkint10   long long
#define unkint11   long long
#define unkint12   long long
#define unkint13   long long
#define unkint14   long long
#define unkint15   long long
#define unkint16   long long

#define unkfloat1   float
#define unkfloat2   float
#define unkfloat3   float
#define unkfloat5   double
#define unkfloat6   double
#define unkfloat7   double
#define unkfloat9   long double
#define unkfloat11   long double
#define unkfloat12   long double
#define unkfloat13   long double
#define unkfloat14   long double
#define unkfloat15   long double
#define unkfloat16   long double

#define BADSPACEBASE   void
#define code   void

typedef ulong size_t;

typedef struct Elf64_Rela Elf64_Rela, *PElf64_Rela;

struct Elf64_Rela {
    qword r_offset; // location to apply the relocation action
    qword r_info; // the symbol table index and the type of relocation
    qword r_addend; // a constant addend used to compute the relocatable field value
};

typedef struct Elf64_Shdr Elf64_Shdr, *PElf64_Shdr;

typedef enum Elf_SectionHeaderType {
    SHT_SYMTAB=2,
    SHT_GNU_versym=1879048191,
    SHT_GNU_verdef=1879048189,
    SHT_GNU_LIBLIST=1879048183,
    SHT_FINI_ARRAY=15,
    SHT_GROUP=17,
    SHT_CHECKSUM=1879048184,
    SHT_SHLIB=10,
    SHT_ANDROID_RELA=1610612738,
    SHT_NOBITS=8,
    SHT_GNU_HASH=1879048182,
    SHT_REL=9,
    SHT_SYMTAB_SHNDX=18,
    SHT_HASH=5,
    SHT_PROGBITS=1,
    SHT_ANDROID_REL=1610612737,
    SHT_NULL=0,
    SHT_GNU_verneed=1879048190,
    SHT_INIT_ARRAY=14,
    SHT_NOTE=7,
    SHT_PREINIT_ARRAY=16,
    SHT_STRTAB=3,
    SHT_RELA=4,
    SHT_SUNW_COMDAT=1879048187,
    SHT_GNU_ATTRIBUTES=1879048181,
    SHT_DYNSYM=11,
    SHT_SUNW_syminfo=1879048188,
    SHT_DYNAMIC=6,
    SHT_SUNW_move=1879048186
} Elf_SectionHeaderType;

struct Elf64_Shdr {
    dword sh_name;
    enum Elf_SectionHeaderType sh_type;
    qword sh_flags;
    qword sh_addr;
    qword sh_offset;
    qword sh_size;
    dword sh_link;
    dword sh_info;
    qword sh_addralign;
    qword sh_entsize;
};

typedef struct Elf64_Sym Elf64_Sym, *PElf64_Sym;

struct Elf64_Sym {
    dword st_name;
    byte st_info;
    byte st_other;
    word st_shndx;
    qword st_value;
    qword st_size;
};

typedef struct Elf64_Ehdr Elf64_Ehdr, *PElf64_Ehdr;

struct Elf64_Ehdr {
    byte e_ident_magic_num;
    char e_ident_magic_str[3];
    byte e_ident_class;
    byte e_ident_data;
    byte e_ident_version;
    byte e_ident_osabi;
    byte e_ident_abiversion;
    byte e_ident_pad[7];
    word e_type;
    word e_machine;
    dword e_version;
    qword e_entry;
    qword e_phoff;
    qword e_shoff;
    dword e_flags;
    word e_ehsize;
    word e_phentsize;
    word e_phnum;
    word e_shentsize;
    word e_shnum;
    word e_shstrndx;
};

#define __WORDSIZE 64

#define __GLIBC_HAVE_LONG_LONG 1




int ifio_060fd.d43e212ce267161a42148cd2a5988a03bf9.3.2.16.1731(char *param_1,char *param_2);
char * ifio_509cc.7a010f16d611d0676da38d5227b43f8bb93.3.2.16.1731(char *param_1,char *param_2);
char * ifio_5f75b.b3daea123684447d30e3c4591a2bf25f92f.3.2.16.1731(char *param_1,char param_2);
char * ifio_e3a35.eb5b432046df145a4cdcd42d87f3c471fde.3.2.16.1731(char *param_1,char *param_2,char **param_3);
int ifio_4bb94.16f9b5d911534abdc39e287b9b326c16a2a.3.2.16.1731(char *param_1,int *param_2);
undefined8 ifio_95ccf.5d0203fb2c3f4113c3ca1519710cd01e9ba.3.2.16.1731(long param_1);
undefined8 ifio_56109.6e22866debab89db1b13d93aabcfb0ad140.3.2.16.1731(long param_1);
void ifio_fdfbe.ae48f92914b5b7dd9b7ae2a3d140240aa37.3.2.16.1731(long param_1,long param_2);
void ifio_113bc.76c819cd57ddc034605f7092d99b9bfe92c.3.2.16.1731(long param_1,long param_2);
void ifio_94d6e.7ac2ac6ccc2b924918580bc9ed9f39acc9c.3.2.16.1731(long param_1);
void ifio_bc2e5.831d3964391fb872e7bb7bacc3286dd6bf8.3.2.16.1731(void);
undefined4 fiocfg_get(long param_1);
undefined8 ifio_ac662.7803489b1e59bacf876c2975de30adcf057.3.2.16.1731(long param_1,char param_2);
undefined4 fiocfg_set(long param_1);
undefined8 fiocfg_enum_get_size(int *param_1);
undefined8 fiocfg_enumerate(uint *param_1,undefined *param_2);
undefined8 fio_config_table_init(void);
void ifio_adc7f.06da7a05b12b19b98314f22a37db750400c.3.2.16.1731(void);
void ifio_e83a9.e463dfe6863baaaab75199dc8ce4de3cbb4.3.2.16.1731(long param_1,uint param_2,int param_3);
bool ifio_115ad.4971df39c4f0a1d96a294d2cb2130d8af16.3.2.16.1731(long param_1,uint param_2,int param_3);
bool ifio_b7933.23c995f0ad9031f0eca793c2850b43fa124.3.2.16.1731(long param_1,uint param_2,int param_3);
void ifio_ae88c.ecd21e183b828dd0264cdfd9f5b151cc324.3.2.16.1731(long param_1,uint param_2,int param_3);
int ifio_a4e27.3efd6c051ad85274a66481fb54c715317f7.3.2.16.1731(long param_1,uint param_2,uint *param_3,int param_4);
uint ifio_e01ff.cd0d9b091c4606371e9b7f179cb218c6e8b.3.2.16.1731(long param_1,uint param_2,uint *param_3);
undefined8 ifio_9c024.4bc417461db07dd67c5172845dff58cd9c8.3.2.16.1731(void);
ulong ifio_e774e.cc5d0b669c542943845de5a87d5285b4d4e.3.2.16.1731(long param_1,long *param_2,undefined8 param_3,int param_4);
undefined8 ifio_551ee.89b6f83b62f4d4049733d15a685ebb2a797.3.2.16.1731(undefined8 *param_1,int param_2);
undefined8 ifio_0cecb.16a2328bdb182e7ebffd0f55988637b6c02.3.2.16.1731(long param_1,undefined4 param_2);
undefined8 ifio_39207.722fab4acd0bba445a8038866e56354107b.3.2.16.1731(long param_1);
void ifio_cc8d6.51936efbeff1191fb5eb8eb149e5f6b280a.3.2.16.1731(long param_1);
int ifio_f4b75.38d444ba41527e778f3fb7e35117b01068b.3.2.16.1731(long param_1);
uint ifio_f5da4.18796c58111c42116d8fd64044e97ab7f3e.3.2.16.1731(long param_1,long param_2,int *param_3,int param_4);
undefined8 dbgs_create_flags_dir(void);
undefined8 dbgs_delete_flags_dir(long **param_1);
undefined4 fusion_register_device(long *param_1);
undefined8 fusion_unregister_device(long *param_1);
undefined8 fusion_do_shutdown(void);
undefined8 FUN_00103070(long param_1,undefined8 param_2,long param_3);
undefined8 FUN_00103140(long param_1,undefined8 param_2,long param_3);
undefined8 FUN_00103240(undefined8 param_1,undefined8 param_2,long param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6);
ulong ifio_7653c.c920cb5012983c87a18a599e03ac78cdbe9.3.2.16.1731(void);
undefined8 ifio_0cb84.6cfb3a1af965689c96d32bac19bde703914.3.2.16.1731(void);
ulong ifio_803ac.ab9633888b6c261f7184ec259c978e56295.3.2.16.1731(long param_1);
undefined8 ifio_94d08.9204402778bf5d743e69d46873fdb9dd1b9.3.2.16.1731(long param_1);
undefined4 ifio_25d3e.3dba6a4488f3f908c3a37209d7d56794a08.3.2.16.1731(long *param_1,long *param_2,long *param_3,undefined8 param_4,uint param_5,uint param_6);
void ifio_05f6c.eecc69e74f20d5252697edfc3832afcc232.3.2.16.1731(long param_1,long *param_2,undefined *param_3,undefined4 param_4,uint param_5);
undefined4 ifio_80eef.9c5ace2d42bc994d3457874f1eb154b0850.3.2.16.1731(long **param_1,undefined *param_2,undefined4 param_3,uint param_4,long param_5,long param_6);
void ifio_4aec5.ebef69faaf43e2f0393b7f00a6cb6b666f5.3.2.16.1731(long param_1);
void ifio_d0bee.26d94578132787b75428f70ffe452c97ae8.3.2.16.1731(long param_1,undefined8 param_2,undefined4 param_3);
undefined4 ifio_ab76f.ccb3ac18694a7dd66bc1ee4110669e8b5ed.3.2.16.1731(long *param_1,ulong param_2,long param_3,uint param_4,uint param_5);
undefined4 ifio_27334.573678fdd19bf65461553e7a52b58acc189.3.2.16.1731(long *param_1,undefined8 param_2,long *param_3,undefined8 param_4,uint param_5,long param_6,undefined4 param_7);
int ifio_41233.4b539bd4b0ce1c43ebbef28fcc415b41e64.3.2.16.1731(long *param_1,undefined8 param_2,long *param_3,undefined8 param_4,uint param_5,long param_6,undefined param_7,long param_8,long param_9,undefined4 param_10);
undefined8 FUN_00103e70(long param_1,long *param_2);
undefined4 kfio_info_node_get_type(long param_1);
undefined8 kfio_info_node_get_size(long param_1);
undefined4 kfio_info_node_get_mode(long param_1);
long kfio_info_node_get_name(long param_1);
undefined8 ifio_17c9f.b1e86f4e0eca7be3687debbf2ac9cbcd366.3.2.16.1731(long param_1);
undefined8 kfio_info_node_get_os_private(long param_1);
void kfio_info_node_set_os_private(long param_1,undefined8 param_2);
ulong kfio_info_create_dir(long param_1,undefined8 param_2,long **param_3);
void kfio_info_remove_node(long **param_1);
ulong ifio_7b07b.46cacdaa1a4cff2bfbbcfb2f22ef7c5a5a8.3.2.16.1731(long param_1,undefined8 param_2,int param_3,undefined4 param_4,long param_5,long param_6);
ulong kfio_info_create_proc(long param_1,undefined8 param_2,int param_3,undefined4 param_4,long param_5,long param_6,long param_7);
ulong kfio_info_create_seqf(long param_1,undefined8 param_2,undefined4 param_3,long param_4,long param_5);
ulong kfio_info_create_text(long param_1,undefined8 param_2,undefined4 param_3,long param_4,long param_5);
undefined8 ifio_21fe6.bc2481b61e63362b67dca23955f8fca002d.3.2.16.1731(long param_1);
undefined4 ifio_7eb4c.e368bde2fdca7dacfac66ba18587ae81fe8.3.2.16.1731(long param_1);
undefined8 kfio_info_alloc_data_handle(undefined8 param_1,undefined8 param_2,undefined8 param_3,long *param_4);
void ifio_2cac8.785de669977689e4186162a963dad6dff8f.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,undefined8 *param_4);
void kfio_info_free_data_handle(long param_1);
long kfio_info_printf(long param_1,undefined8 param_2,undefined8 param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6,undefined param_7);
ulong ifio_fab2f.b26dcb8a346e767190cb4b832b80c6b8ab4.3.2.16.1731(long param_1,undefined8 param_2,ulong param_3);
undefined8 kfio_info_data_node(undefined8 *param_1);
bool ifio_a4433.89173acad6fd81c1a1a4038ee9880c27126.3.2.16.1731(long param_1);
undefined8 ifio_e0c7c.f1401b575a06316731c1d34242b4b63d484.3.2.16.1731(long param_1);
void ifio_9de7e.b6d304bf5ecd422f9408e70c60b7196cd20.3.2.16.1731(long param_1,undefined8 param_2);
undefined8 kfio_info_data_size_valid(long param_1);
undefined8 kfio_info_data_size_written(long param_1);
long ifio_0093b.8d791372ddcd6afe56ae850be957ed0198e.3.2.16.1731(long param_1);
bool kfio_info_data_get_eof(long param_1);
void ifio_d4dfb.83d5780d46eb3be1883702296c36977fa5d.3.2.16.1731(long param_1,int param_2);
void kfio_info_seq_init(long param_1);
void kfio_info_seq_next(long param_1);
void kfio_info_seq_stop(long param_1);
void kfio_info_seq_show(long param_1);
undefined8 kfio_info_handle_cmd(undefined8 param_1,long param_2,long param_3,undefined8 param_4);
undefined8 kfio_info_generic_type_handler(long param_1,undefined8 param_2,long param_3,long param_4);
undefined8 kfio_info_generic_text_handler(long param_1,undefined8 param_2);
uint ifio_98367.73da72ba288b95718c0f5555889d0765a0d.3.2.16.1731(char *param_1,undefined8 param_2);
undefined8 FUN_001056d0(undefined8 param_1,uint *param_2);
undefined8 FUN_001057d0(undefined8 param_1,uint *param_2);
undefined8 ifio_1fbaf.f0fc06c029ebb7870a6faa7deba5241a3ab.3.2.16.1731(undefined8 param_1,undefined *param_2,uint *param_3);
ulong ifio_eb321.48832edcca1752c6422d78bfa754c46f867.3.2.16.1731(long param_1,long param_2);
ulong ifio_7e3a3.ed40884c3016d13c6a23dedee8f561fbc42.3.2.16.1731(long param_1,long param_2);
ulong FUN_00105cd0(long param_1,long param_2);
undefined1 * fusion_nand_get_first(void);
undefined1 * fusion_nand_get_next(long *param_1);
long fusion_nand_get_miscdev(long param_1);
undefined4 fusion_nand_get_devnum(long param_1);
char * fusion_nand_get_bus_name(long param_1);
char * fusion_nand_get_dev_name(long param_1);
undefined8 fusion_nand_get_pci_dev(long param_1);
bool is_nand_device_event_set(long param_1);
long nand_device_get_poll_struct(long param_1);
undefined8 ifio_35167.580036aebd9269c230d43011467c0842672.3.2.16.1731(uint param_1);
undefined8 ifio_8764e.881c531c573582fb68e23309776b883108a.3.2.16.1731(void);
undefined8 ifio_89423.82d4d97c31d90a372cd2fc34e7409cb2ca1.3.2.16.1731(long param_1,undefined8 param_2);
ulong ifio_1bf9c.a7ef426bde893c564fe4bb90a88a726ada1.3.2.16.1731(long param_1,undefined8 param_2);
int ifio_aa453.998ae97ef21dcc6efddd634e67aed8ec37e.3.2.16.1731(long param_1,undefined8 param_2);
ulong ifio_7e117.be8578421a8fd63066bcb88ee884c4d8452.3.2.16.1731(long param_1,undefined8 param_2);
int ifio_ea611.55aac780b851d114cb6017458ebf2b9f74a.3.2.16.1731(undefined8 param_1,long param_2,undefined8 param_3,long *param_4);
undefined4 ifio_e6a72.5290870a5a3a6dd234c0a5b81ae62674186.3.2.16.1731(undefined8 param_1,long param_2,undefined8 param_3,long param_4);
uint ifio_ba31e.77a8b2e9ad993f313f08144de9d5a4d8ff7.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
undefined8 ifio_75cc2.5cd55499707f984f33d9fa744b70e35189b.3.2.16.1731(void);
int ifio_61801.dd8324e0cf1d297152175a2415f56357b20.3.2.16.1731(undefined8 param_1);
int ifio_981b1.e2b7bd76c66518f7df03a6ef11237d0b8b0.3.2.16.1731(undefined8 param_1,long param_2);
undefined8 ifio_c108f.ebcf8af8d27cda93fc864ad68565e8bf8ba.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
undefined8 ifio_f4119.3743922905c21d408bbde566e4872943012.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
undefined8 ifio_a141b.9cb730d0e02e60b1097d5f50e64b67c8ef7.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
undefined8 ifio_f09dc.72451d426e891ae8edf2631b383f66e5f25.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
int ifio_e66bf.37f9bd803884fb3b74400458b7e884b9c5a.3.2.16.1731(undefined8 param_1,long param_2,long param_3);
int ifio_bde10.d8ff6fc8f867bac5de74c3b5b30c79b8a11.3.2.16.1731(undefined8 param_1,long param_2);
int ifio_dfd7b.c65b2ee4cd39b6c7422fd0d64486431e0dc.3.2.16.1731(undefined8 param_1,undefined8 param_2,long param_3);
undefined8 ifio_1a7e9.52677600cb195a67c4fc13a3f93324ecb6a.3.2.16.1731(undefined8 param_1,long param_2,long param_3);
undefined8 ifio_022b7.384044e3223141b351d0708f9d5de9b3751.3.2.16.1731(undefined8 param_1,long param_2);
undefined8 ifio_0611f.11c4c2ad059a101847cc4a1f18521518b45.3.2.16.1731(undefined8 param_1,long param_2);
undefined8 ifio_84241.37783e092e673a2f8c7c07b0e9cea4a374d.3.2.16.1731(undefined8 param_1,long param_2);
void ifio_9d923.7ca8c75a663d2b892503fb9c1f49e9d2bc2.3.2.16.1731(long param_1,long param_2);
int ifio_f8610.ac580983db7538817f515c600812c315444.3.2.16.1731(undefined8 param_1,long param_2,undefined8 param_3,long param_4);
undefined8 ifio_44807.e4eb15bd100163390c901b8c68e23d29903.3.2.16.1731(void);
int ifio_fbfae.5f21bdd30b1b62fc23b4b8d6d7181bfee3a.3.2.16.1731(undefined8 param_1);
int ifio_c1300.b1ff2ba4d0273ac0ddb35119db484845a9f.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
void ifio_fc64f.6bd3876b0649649013bd4e5867882466e2f.3.2.16.1731(undefined8 param_1,long param_2,long param_3,long param_4);
uint ifio_8e273.7f0f8ea19860a99787bd5ad84c25af99c1c.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
int ifio_5f96f.2bfecf410ddb8cefd61c806be6457a35abe.3.2.16.1731(undefined8 param_1,undefined8 param_2,long param_3,long param_4);
int ifio_e3a3f.45315794af5f0e843f9a1b5039a9b446657.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
int ifio_d77f0.f2124cbe5d1c85556e845dd6b06c9df739b.3.2.16.1731(undefined8 param_1,long param_2,long param_3,long param_4);
undefined8 ifio_5271f.10f8baf32b580cd5a427c83092891c0caaa.3.2.16.1731(undefined8 param_1,long param_2);
undefined8 ifio_27d3e.bdc51b02ee1baa8882a337bea825dffbef6.3.2.16.1731(undefined8 param_1,long param_2);
int ifio_2e089.839e9eb1a7d8ada269c289c510555e350d4.3.2.16.1731(undefined8 param_1,long param_2);
ulong ifio_87251.4684409a28f3570f58c1d3f30aaa6ddf6d7.3.2.16.1731(undefined8 param_1,undefined8 param_2,long param_3);
undefined4 ifio_97ed7.4b3397be5a56470db2570af1c906f876c3a.3.2.16.1731(undefined8 param_1,long param_2,long param_3,long param_4);
int ifio_d805c.a2fa1b429bb2a5ed5c63d7f58e9c4d65b03.3.2.16.1731(undefined8 param_1,long param_2,long param_3,long param_4);
undefined8 ifio_16cc6.579f98fda8cc45bfb6cfabe22ece50904c2.3.2.16.1731(undefined8 param_1,long param_2,undefined8 param_3,long param_4);
int ifio_cc7dc.22b5668c69919119146a63827dbe1b3a67c.3.2.16.1731(undefined8 param_1,long param_2);
int ifio_682b8.e3770363fca23e9589c7875a93603e5d8b5.3.2.16.1731(undefined8 param_1,long param_2);
undefined8 ifio_7cef3.89dd68a8b4982f867cefcda44977a040551.3.2.16.1731(undefined8 param_1,undefined8 param_2,long param_3);
undefined8 ifio_d4799.3df45c080b7de7099cec36faf693264e449.3.2.16.1731(undefined8 param_1,undefined8 param_2,long param_3);
int ifio_7acc8.5cb91028121a3fb15feda027efbc6e10308.3.2.16.1731(undefined8 param_1,long param_2,long param_3,long param_4);
undefined4 ifio_5d474.2a0ee8926545665444cb056a6caf2ba6ec3.3.2.16.1731(undefined8 param_1);
undefined4 ifio_25419.60ba1f64164dd4eef4f392ae6f052583d62.3.2.16.1731(undefined8 param_1);
ulong ifio_3e1cf.4458748fa15d7918dfad1f582943dd97799.3.2.16.1731(undefined8 param_1);
ulong ifio_b7d9d.74ccceb6f1df4338b0aba3aa9a33de9e23c.3.2.16.1731(long param_1);
int ifio_e7abc.70340dd4b1744d06c7babb4516312d9e2b2.3.2.16.1731(undefined8 param_1,long param_2,long param_3,long param_4);
ulong ifio_ae26b.c1a6de90be3a7cb1617ccf847f2c6d0bdee.3.2.16.1731(long param_1,long param_2,long param_3);
undefined8 ifio_6d392.c827a208554c6521acf0845eb8dda7d72b6.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
undefined4 ifio_75422.ba69262692bac4347c86ca7232f49043de6.3.2.16.1731(undefined8 param_1,long param_2);
undefined8 ifio_df239.b69f3aa7c10b71b92ddc4250713fdcde16f.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
uint ifio_47932.c947484e07e782c3d821a33667551470c41.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long *param_4);
undefined8 ifio_bc2b9.8b2bc5cc57b4473daad9917f584ed2a20b4.3.2.16.1731(undefined8 param_1,long param_2,undefined8 param_3,long param_4);
undefined8 ifio_573c9.6c51941fe0d79eac94e90c35869a223b0be.3.2.16.1731(undefined8 param_1,long param_2,undefined8 param_3,long param_4);
void ifio_d55bf.e53b9be2259a8c725191388361bba3229e5.3.2.16.1731(long param_1,undefined8 param_2,undefined8 param_3,long param_4);
void ifio_a4f7a.40fc27127d61052710579fa3c5f0c449ba4.3.2.16.1731(void);
void ifio_ab8d1.2dbfed49a55b72ee96aff9986e34205025a.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long *param_4);
undefined8 ifio_16603.7f2b797b3ac5a423fc09331ae8e7ff8231a.3.2.16.1731(void);
undefined8 ifio_19d9d.5d79b2682ad9b3d921aeb4d0b3a99752fe3.3.2.16.1731(undefined8 param_1,long param_2,undefined8 param_3,long param_4);
undefined8 ifio_0eba2.b430925cd9161f6f13a8378146edce3e881.3.2.16.1731(void);
undefined8 ifio_424b1.7d7c1d6027195d2685eeb9c4aca2539f8b7.3.2.16.1731(undefined8 param_1,undefined8 param_2,long param_3);
undefined8 ifio_a78b4.5fa1b65f6dcf7ecc2801732fdd07c8aaebb.3.2.16.1731(undefined8 param_1,undefined8 param_2,long param_3);
undefined8 ifio_86bb1.4520a7a5f3d2c4d372cb247112c4809024c.3.2.16.1731(void);
undefined8 ifio_4dda4.0e86c4a20326fcf2f4ac19d77868f41db65.3.2.16.1731(void);
int ifio_046fc.3b75dca126e99f659d8d173442cf43f49b8.3.2.16.1731(undefined8 param_1,long param_2);
undefined8 ifio_e82f8.655a80a1602e7ef3bbc91ae2e3c102c6914.3.2.16.1731(undefined8 param_1,long param_2,undefined8 param_3,long param_4);
undefined8 ifio_2e6b0.957a1df2a8dbe01b98fa61e6d28cca5da8e.3.2.16.1731(undefined8 param_1,long param_2,undefined8 param_3,long param_4);
undefined8 ifio_4d31c.20c190a04e3c8d18c614e4118f7cb6ce213.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
int ifio_10f3c.e431204b57dad108626c77c0d76a6ef592a.3.2.16.1731(undefined8 param_1,long param_2);
undefined8 ifio_32750.3f05e1b6f2839d1a6e9e6cd386b3547a63a.3.2.16.1731(void);
int ifio_b1d4e.5ab8b2ba6961b238ea2b4f65f910f4154cc.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
undefined8 ifio_7386e.c867b5c8a65ec35e962e9fd460bb32da5a5.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,char *param_4);
int ifio_0759c.2d75ff36cbd590c8aaf24e262407e28a7f0.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
int ifio_bccb1.296199045c0c7063898ce72fc4ca7024b3e.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
ulong ifio_fd103.d57dc8f81e10d2c545b345d08340c4f2363.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
undefined4 ifio_904e5.106afbf70bf7cf6fa21e45aa9e419229c64.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
undefined4 ifio_74cd7.0215008b805ae5739e6e78061d33ff1f167.3.2.16.1731(undefined8 param_1,long param_2,undefined8 param_3,long param_4);
uint ifio_5fb73.022d8a509c63ef99a4f55c1ea4abb4b9d57.3.2.16.1731(long param_1,undefined8 param_2,undefined8 param_3,long param_4);
void ifio_73957.5aad03451b5de981d3da1a667138330fdee.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
undefined8 ifio_8c03b.40621ee97b9b2a074b171fc60e2a810ac4e.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
ulong ifio_5ceac.f6df950588780af0cb6741049ae3170e522.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
ulong ifio_896e8.88b549cf916928672ef3dc258bdcd2cb097.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
ulong ifio_0a5d7.7f3b3e0d3a84dbc7aa14958a8291f94ba1b.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
int ifio_977f4.d69c98c08ede2b2beabc9e4520f2767fecd.3.2.16.1731(long param_1,undefined8 param_2,undefined8 param_3,long param_4);
ulong ifio_f126d.0be484dab70a6ccba7054530c0091cb1b98.3.2.16.1731(long param_1,undefined8 param_2,undefined8 param_3,long param_4);
int ifio_8fcb8.8c9686b24678299fa24d3c912e1fb7f0f6d.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
uint ifio_2455e.4d4df9a4ae8199f504930f64827de27aab4.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
uint ifio_2996b.793b4e9178f51f37eabc75a2d77fcdb1b71.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
ulong ifio_7f9b7.6b035a24a3b023bc82e5ac467fca3a80686.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
ulong ifio_f796a.772b41e3d301630e6a5ce15f6e65d81c3e1.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
ulong ifio_7f120.8cbff966d0b466e033011e23c65d21900e9.3.2.16.1731(undefined8 param_1,undefined8 param_2,long param_3);
undefined8 ifio_7a100.c42fa5cf118819efa938f30843996eb1579.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
undefined8 ifio_cb226.0804ac62973713b6364d34451ce49936288.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
undefined8 ifio_b3d25.afd73a607f1c80f5040557a6f959e92cd58.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
undefined8 ifio_84f72.4584e4738cd528665e48dbade5e32f5f497.3.2.16.1731(void);
undefined8 ifio_82f46.26e02162de7ff4b8073a749da35e5d50676.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
ulong ifio_e1979.74f2a27aec00c7680865f9334888b89d4e8.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
int ifio_ac23b.35d8915118cce2e9ea7189a16dcc024ce42.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
int ifio_65641.a9d1e02bcce9085d479f3d4913fea9b27a0.3.2.16.1731(undefined8 param_1,undefined8 param_2,long param_3,long param_4);
undefined8 ifio_e384c.4715c679d5d5be52358ae1b53cf432400ff.3.2.16.1731(undefined8 param_1,long param_2,long param_3,long param_4);
int ifio_86da0.fc34bf349a1a0df46b44caa2746a6a76934.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
int ifio_751fb.aa1417dd957d7e3731310c2a97829f02de3.3.2.16.1731(undefined8 param_1,long param_2);
undefined4 ifio_edf18.272a5656acc603a00563a831c1eeffaf805.3.2.16.1731(undefined8 param_1,long param_2,undefined8 param_3,long param_4);
int ifio_41a4d.63f6d9c7c1d427e5f14e7c035f4acda6162.3.2.16.1731(undefined8 param_1,long param_2);
undefined8 ifio_a1663.4e67dc51b8a6db6a5e120802369965c6f88.3.2.16.1731(void);
undefined8 ifio_971af.bfc234c3a114e2b165cae6ff191c01dbc59.3.2.16.1731(void);
ulong ifio_69f7a.c7189e7578f8e1820c03171cd9e52fd0066.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long *param_4);
undefined8 ifio_edd30.9befa7d510667160cd815776ce4e1f0ca9f.3.2.16.1731(void);
ulong ifio_495a4.f12956e500a76dc2ac8a11b765fcb53cff6.3.2.16.1731(undefined8 param_1,undefined8 param_2,long param_3);
int ifio_52b50.c4dc1ea8902433c7f38647ce94a1b826eb4.3.2.16.1731(undefined8 param_1,undefined8 param_2,long param_3);
ulong ifio_233a7.06cadd5d25510396bf7a5eaf6876036d3d8.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
int ifio_e27fa.0e52129df54665da67bc4f2fae57cedc9bc.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
int ifio_43648.e3cbbc93ec7b76b2e1cf8a5847054736d98.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
int ifio_8f544.3eabfecddcc1bae7025fad8e4dc24caeba7.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
undefined8 ifio_99fd5.5e74c557db5a5ad06ed98540ba6203d95fb.3.2.16.1731(void);
undefined8 ifio_715c4.008f109f6fd9fe160d89c98342dd7b81d40.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
undefined8 ifio_228c8.a770a2f376bf6f6e057c4f4ed80a6c03ee9.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
undefined8 ifio_92976.d80c1ae3125ac16ddf5355815ec8bbab056.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3,long param_4);
int ifio_327c6.2ab15e9257c4391eea0c419b80e6114e84c.3.2.16.1731(long param_1,undefined8 param_2,undefined8 param_3,long param_4);
ulong ifio_1031e.cd6df735602f36f4e3c833b3b56bd2147ae.3.2.16.1731(undefined8 param_1,long param_2,long param_3);
ulong ifio_2a1aa.9056ff892fa146a3c24859394ae2a4fd171.3.2.16.1731(undefined8 param_1,long param_2,long param_3,long param_4);
ulong fusion_control_ioctl(long param_1,uint param_2,long param_3,char *param_4);
void ifio_ed475.2aa2f5de054e43121b8f7c7e6045dff5c92.3.2.16.1731(long param_1,uint param_2);
char ifio_b5888.110269ef14e1e8752fdf223c6cb56f533fc.3.2.16.1731(long *param_1,ulong param_2);
undefined4 ifio_bce8b.65d17fc3318dba135c9c9d80fc7580c718e.3.2.16.1731(long **param_1,long *param_2,long param_3,long param_4);
undefined4 ifio_612c1.de67fe9c862388a5ebcf4bb0862bd5fe3ac.3.2.16.1731(long **param_1,long *param_2,long param_3);
undefined4 ifio_39b6d.5e94cf82fedc768b6e38f93696030605866.3.2.16.1731(long **param_1,uint param_2);
undefined4 ifio_3dede.992f22b599579b34404f1af663db42b06bd.3.2.16.1731(long *param_1,long param_2,long *param_3);
undefined4 ifio_6b333.b114f68fbb51c7c80fd77478726ed889c55.3.2.16.1731(long *param_1,long *param_2,undefined2 param_3,undefined2 *param_4);
undefined4 ifio_171c7.e0980f89af8a9619f7f7ce4a01fcdede2dc.3.2.16.1731(long *param_1,long *param_2,undefined2 param_3,undefined2 *param_4);
undefined4 ifio_f151a.b33f098d8ee7baa75ad5255484ff3d3f9d0.3.2.16.1731(long *param_1,undefined2 param_2,long param_3,uint param_4);
undefined4 ifio_60930.9cb0c79b9aa22c98b761fe7e6dde85cc975.3.2.16.1731(long *param_1,undefined8 param_2,long *param_3,undefined4 param_4,long param_5);
void ifio_d0a4c.78a5e34fbe080b04395917c361077510107.3.2.16.1731(long param_1,uint param_2);
void ifio_b2304.80df6fe07e21c1831264941a37d9bc71949.3.2.16.1731(long param_1,undefined8 *param_2,undefined param_3,int param_4,undefined8 param_5);
undefined8 init_fio_dev(void);
void cleanup_fio_dev(void);
char * ifio_2ccfa.b292ea16d62db11c6e7f9fda42894a80414.3.2.16.1731(uint param_1);
undefined * ifio_ad931.17458504da847404b0a49738ba395f7d2cc.3.2.16.1731(uint param_1);
void ifio_d7479.489333bf4f54b8a4d5957f9e000c9aa27c0.3.2.16.1731(long param_1,undefined8 param_2,undefined4 param_3);
char * fusion_request_get_bus_name(long *param_1);
uint ifio_f2de5.480243a39ce4f0a32d8489ce08784458030.3.2.16.1731(long param_1);
void ifio_5b134.744ef3df67b019ccfa084a705d8e50ec01f.3.2.16.1731(long param_1);
void ifio_c3970.1f0904f9a490772ae2f2b178a8987824c63.3.2.16.1731(long param_1);
void fio_uuid_copy(undefined8 param_1,undefined8 param_2);
void ifio_5c537.7f0529336f888b8d92bd1ee41bf0df18249.3.2.16.1731(undefined8 param_1);
void fio_uuid_compare(undefined8 param_1,undefined8 param_2);
bool fio_uuid_is_null(undefined8 param_1);
void fio_uuid_unparse(ushort *param_1,undefined8 param_2);
undefined4 ifio_2cc92.ab4c2f5f217d6ff63432a902c3809a9a3a4.3.2.16.1731(long param_1,ushort param_2,uint param_3);
undefined4 ifio_0e116.91f02205aa3637102e72dd61c7725c231b4.3.2.16.1731(long param_1,ushort param_2,uint param_3);
int ifio_083d2.ca72df5c21d9abd774c0a193f43726446b7.3.2.16.1731(long param_1,ushort param_2,uint param_3);
int ifio_40575.9813003f095140ea18dff0bd300b20b7b5d.3.2.16.1731(long param_1,ushort param_2,uint param_3,int param_4);
int ifio_5e634.43178070070b57ea2233d4de6abb284c263.3.2.16.1731(long param_1,ushort param_2,uint param_3,int param_4);
int ifio_d6627.008938d1aa8ccc8fe0e546d8941e27efe59.3.2.16.1731(long param_1,ushort param_2,uint param_3,int param_4);
undefined4 ifio_5abc5.9e9e9bae61a2d1b318a5fc870f679a9acd2.3.2.16.1731(long param_1,ushort param_2,uint param_3);
ulong ifio_08ac9.d7909e5d15adcf6d1ad567188e7f2812c0e.3.2.16.1731(long param_1,ushort param_2,uint param_3);
void ifio_1a01c.11c3be21742d85d64c328985513fb741fff.3.2.16.1731(long param_1,ushort param_2);
int ifio_89b7c.7546f2ad11a5dfebb05ffb08ed3e02ae8d3.3.2.16.1731(long param_1,ushort param_2);
int ifio_669d3.67ba52dae34b822ac59e45a2120a0c23add.3.2.16.1731(long param_1,ushort param_2);
undefined4 ifio_be1ac.b90a74dfe66f378c276d45c5595a0bb6e21.3.2.16.1731(long param_1,ushort param_2,ulong param_3);
undefined4 ifio_6553d.14e5fa67e356029bc5c85b39c498d84e758.3.2.16.1731(long param_1,ushort param_2,ulong param_3);
undefined4 ifio_a4003.5e2121fa9792f0d5ef901e032239b6ae5b8.3.2.16.1731(long param_1,ushort param_2,ulong param_3);
undefined4 ifio_9817d.232dd190293f6e9c3f73b46f3a0575cb718.3.2.16.1731(long param_1,ushort param_2,ulong param_3);
void ifio_72353.c364ace6647833284d518481605978e220e.3.2.16.1731(long param_1,ulong param_2,ushort *param_3);
int ifio_b7f40.e9ddd389a453726de8ba9326dda1808371a.3.2.16.1731(long param_1,ushort param_2,uint param_3);
void ifio_3e9da.4dee8c4e26061231f2b026e780647d2a7d3.3.2.16.1731(long param_1,ulong param_2,int *param_3,undefined4 *param_4);
void ifio_34f89.8bae173c898ad307bdad2b9ac7100bfea02.3.2.16.1731(long param_1,ulong param_2,undefined2 *param_3);
void ifio_5d46e.98e2cbae554deeed0c5a6968688847d2893.3.2.16.1731(long param_1,ulong param_2,ushort *param_3);
void ifio_3f8e5.26db25548b48e0bcb2c4e991b87647cf79a.3.2.16.1731(long param_1,ulong param_2,int *param_3);
void ifio_201e2.6f7e8e16817fc5b68c36a4604bbc3998a4c.3.2.16.1731(long param_1,ulong param_2,undefined2 *param_3);
undefined8 ifio_c6b56.0c01e0dc9a22141204ec4b12f9e92e9ef06.3.2.16.1731(long param_1,ushort *param_2,int param_3);
undefined8 ifio_a24ef.f99c44361f2b85b9c6264fa4d0766e6cf95.3.2.16.1731(long param_1,ushort *param_2,int param_3);
uint ifio_7bee6.9047d4637b77740df3c682ddaf6dd7ff881.3.2.16.1731(long param_1,uint *param_2,int param_3);
void ifio_39573.f525024c9e2737d0c4812681524ab552516.3.2.16.1731(long param_1,uint *param_2,int param_3);
int ifio_4ac00.2bab44905f062550701066a4e4953d311c4.3.2.16.1731(long param_1,ulong param_2,int param_3,int param_4,uint *param_5);
undefined  [16]ifio_6c99a.ea543b8fbd42e045ebb1aef4b029e92db0e.3.2.16.1731(long param_1,ulong param_2);
undefined8 ifio_87e3c.181d7866ab3bd6910ef0aad4b357db2c0dc.3.2.16.1731(long param_1,ushort *param_2);
void ifio_fe85e.227c175bb06977d83f22c752d8e80c6167b.3.2.16.1731(long param_1,undefined8 *param_2);
void FUN_00112cf0(long *param_1);
int FUN_00112dd0(long *param_1,long param_2,ulong param_3,int param_4,char param_5,byte param_6,long param_7,long param_8);
void ifio_c366b.0a5cec8c6628913ef05f16492c5e099e982.3.2.16.1731(long param_1);
int ifio_dbb38.72f06d37831695e0e1f85107672423d9242.3.2.16.1731(long *param_1,long *param_2,ushort param_3,long param_4,long param_5,long param_6);
int ifio_16a89.2982a52e5394b29c6e811678eedd7335bd3.3.2.16.1731(long *param_1,long param_2,ushort param_3,long param_4,long param_5,long param_6);
void ifio_a0f76.ae5d683e4d073d7a5a234017355ababf1bc.3.2.16.1731(long *param_1);
void ifio_d9da3.a939f2fd78258c0b57b1f630067f4cd3105.3.2.16.1731(long *param_1,ulong param_2,long param_3);
void ifio_4c4d4.69583a3f195781cde98f0f8fb317d206c8f.3.2.16.1731(long param_1,long param_2,ulong param_3,uint param_4,char param_5);
byte ifio_af499.1e9bf94f13c10fe64072fc930e00b633454.3.2.16.1731(long param_1,ulong param_2,int *param_3,int param_4,byte param_5);
void ifio_81985.829e2daa55bc9519f815d47ac7159d175be.3.2.16.1731(long param_1,ushort param_2);
void ifio_51fab.fd009dcf04d39255eb7e5b38c71778babfb.3.2.16.1731(long *param_1,ushort param_2,long param_3,long param_4);
void ifio_2ea62.2f7e0ebab01c3a268ac778c77b89841d35c.3.2.16.1731(long *param_1,ulong param_2,int param_3,long param_4);
int FUN_00114d80(int *param_1,int param_2);
undefined8 ifio_7d572.9d464188a8a3252c46e49e4665eb7c37fbf.3.2.16.1731(long param_1,long param_2);
int ifio_37482.f03ddaf15c5ba1316c4fd10bfe326139076.3.2.16.1731(long *param_1,int param_2,uint param_3);
uint ifio_4801f.e1415882cd5139d032bdf0a49557aa1c757.3.2.16.1731(long param_1,uint *param_2);
undefined4 fio_detach(long *param_1,uint param_2);
ulong FUN_00116b70(long param_1,uint param_2,undefined8 param_3,char *param_4);
void ifio_64763.3a58b323a2eaeec6b1d2eaae42a11f0f0f8.3.2.16.1731(long param_1,int param_2);
int ifio_8b01a.9b8486e157b44f0600151ced61c85908554.3.2.16.1731(long param_1);
void fio_auto_attach_wait(void);
int ifio_1d82e.51fa10865e9de192e97d387322d0cd5e41e.3.2.16.1731(void);
void fio_shutdown_device(long *param_1);
void ifio_584ab.5ccd2274af8cf6a0942d5cf13cd0df844ed.3.2.16.1731(void);
void FUN_001170b0(long *param_1);
int ifio_ad5ab.8239a36c2d052af86b3de5af7b6ac1cf518.3.2.16.1731(long param_1,undefined8 param_2,uint param_3,int *param_4,uint param_5);
int kfio_handle_atomic(long param_1,undefined8 *param_2,uint param_3,int *param_4,uint param_5);
undefined8 ifio_3128c.cc9981bb932352dde7251e5bb3b39b912cc.3.2.16.1731(long *param_1,ulong param_2,undefined8 param_3);
undefined4 ifio_de717.df3bb6edbb5ff0fb6c4fbc8ef9abd66667c.3.2.16.1731(long param_1);
undefined8 ifio_ba676.9ddf59d07ea2fd61e84d7baa16ce5fa41b6.3.2.16.1731(long param_1,long param_2);
uint ifio_6d61e.42ffb67d2831156b628edddf50d9c4213f1.3.2.16.1731(long param_1,ushort param_2,long param_3);
ulong ifio_354a4.bab51f46e6741955c33dc1f4b4d936af767.3.2.16.1731(long param_1,long param_2);
undefined8 ifio_cfef7.c6ab5da4367c1436fc3fa90c5b7fd7d4b84.3.2.16.1731(undefined8 param_1,int param_2,undefined4 *param_3);
undefined8 fio_open(long param_1);
undefined8 fio_release(long param_1);
void kfio_set_dev_state_running(long param_1);
void kfio_set_dev_state_quiescing(long param_1);
char * fio_device_get_bus_name(long param_1);
undefined8 ifio_bea78.afb924ced80da6c4462d282ebd25f2c7f8e.3.2.16.1731(long param_1,int param_2,char *param_3,char *param_4);
ulong fio_ioctl(long *param_1,uint param_2,undefined4 *param_3,char *param_4);
undefined4 ifio_71b9c.c0c3310ba0e34b00b55444a669c510fd127.3.2.16.1731(long param_1);
undefined4 ifio_58422.95893ea3c09471a1c465f0c2577c35fef60.3.2.16.1731(long param_1);
undefined4 ifio_8fb16.0edc34d825cdc259097d0215ce360f2979d.3.2.16.1731(long param_1);
void ifio_b1de6.f0f2c5c48945500723f05b8cb16f8bbc1de.3.2.16.1731(long param_1,long param_2);
undefined fio_device_ptrim_available(long param_1);
void FUN_00119130(long param_1,undefined8 param_2,long param_3);
void ifio_5f10a.f7fc8c1c50705f4a4bc4dbcf7232a4dc1d8.3.2.16.1731(long param_1,long param_2,ulong param_3,ulong param_4,char param_5,char param_6);
ulong ifio_3f84f.bc7e2b21e2e4e230a5e09f2f5b6badbaf39.3.2.16.1731(long param_1,uint param_2);
void ifio_4de33.fba296c1ace974be7977dd68d240a03edfe.3.2.16.1731(long param_1);
undefined4 fio_handle_discard_bio(long param_1,long param_2);
undefined4 ifio_3d82a.c1da728e63a6c28b564f4128df11011bcec.3.2.16.1731(long *param_1,long **param_2,long *param_3,long param_4,char param_5);
undefined4 ifio_7ad26.38370d556aefdf31e1f64327e230838e58f.3.2.16.1731(long *param_1,long **param_2,long *param_3,long param_4,long *param_5,long *param_6,char param_7);
int ifio_54c5d.d08fd7c4ce89fa61f660f14b81f35f83ff2.3.2.16.1731(long *param_1,undefined8 param_2);
undefined8 ifio_015db.f4472ed7df1b82d80c0d8d7eb68abe47d24.3.2.16.1731(long param_1,long param_2);
void FUN_00119cc0(long **param_1,long param_2,int param_3,long **param_4);
undefined8 FUN_00119f20(undefined8 param_1,ulong param_2,long *param_3);
undefined4 FUN_0011a1a0(long **param_1,long *param_2);
void FUN_0011a3e0(long **param_1,long **param_2);
undefined8 FUN_0011a7b0(long param_1,ulong param_2,long param_3,long **param_4,long **param_5);
undefined8 FUN_0011ac70(ulong param_1,ulong param_2,long **param_3,ulong param_4);
void ifio_27f7e.ecac47cb45d6b9e24930c8ea0872beee398.3.2.16.1731(long param_1,long param_2);
void ifio_b3d22.14bc014faaf502584bfd9b50db68900722b.3.2.16.1731(long *param_1,undefined8 param_2);
int ifio_1635f.adb22b1febb56cbe794d1cf21c8f5da3dba.3.2.16.1731(long *param_1,long *param_2,long param_3,long *param_4);
void FUN_0011b360(long *param_1,undefined8 param_2,long param_3);
int ifio_b0dc1.a0eaaacade448681428f7bad8c5562c6f00.3.2.16.1731(long param_1,long param_2);
int ifio_a0226.afe042b6defdf131b5b7a01f5c55e6db966.3.2.16.1731(long *param_1,long *param_2,long param_3,long *param_4);
long ** kfio_bio_alloc(long *param_1);
bool kfio_bio_should_fail_requests(long param_1);
long ** kfio_bio_try_alloc(long *param_1);
ulong kfio_bio_submit_handle_retryable(long *param_1,undefined8 param_2);
int kfio_bio_submit(long *param_1,undefined8 param_2);
void kfio_bio_free(long param_1,undefined8 param_2);
void ifio_f9142.4bb664afe4728d2c3817c99088f2b5dd004.3.2.16.1731(long param_1,undefined8 param_2,undefined4 param_3);
undefined4 ifio_8a009.092e9ecf64ba7edb1a4b23a4fa675fda971.3.2.16.1731(long param_1);
void kfio_dump_fbio(long *param_1);
undefined8 ifio_ceecf.95023bfc5faffe9251eee8edd5b1065480f.3.2.16.1731(long param_1);
undefined8 ifio_9b60f.b8b4c7d52cd61f495fd0edcddecda8134ee.3.2.16.1731(long param_1);
int ifio_16616.5535a7bf963c2718315a26e2dc3a640474e.3.2.16.1731(long *param_1,long **param_2,undefined4 *param_3,long *param_4,long *param_5);
int ifio_a4efb.2391332f43b92835baecdeecd48e915d64b.3.2.16.1731(long *param_1);
int ifio_5182e.fc26151a90546dc1565f11a551545b33113.3.2.16.1731(long *param_1,long param_2,ulong param_3,char **param_4);
int ifio_9a956.075aaebbd4ec0e88e18f221b1e679a33c81.3.2.16.1731(long *param_1,long param_2,ulong param_3,long *param_4,char param_5);
int ifio_9ff62.365c5ca38f4ce33bce9148d3dc0b33f4d8b.3.2.16.1731(long *param_1);
int ifio_6b345.51b9d61fa17d6f9ad72b506c024144dfc28.3.2.16.1731(long *param_1,ulong param_2);
undefined8 ifio_4b806.4a3d2cbeba6be3aab98a93818d2e118f2bb.3.2.16.1731(long param_1,long *param_2);
undefined8 ifio_e4ac0.95af33c028b7e2d15617001289a8f24fbcd.3.2.16.1731(long param_1,long *param_2);
void FUN_0011cea0(long param_1);
ulong FUN_0011d1d0(long param_1,ulong param_2,uint param_3,uint param_4,long param_5);
undefined8 ifio_04fe6.427882b2b9fabf3de26f46e51c77b7d2353.3.2.16.1731(undefined8 param_1,ulong param_2,long param_3,long param_4);
void FUN_0011d910(long param_1);
void FUN_0011daf0(long param_1,undefined8 param_2);
void ifio_e9f68.eca07c4a68431c6b9c0816ac5b92f970cd6.3.2.16.1731(long param_1,long param_2);
long * ifio_af1ed.c6b2e1686680a7d09443645de32c0874098.3.2.16.1731(long param_1);
void ifio_c0d9d.3bbdc40a6a52944a69ddf264a80c5de1820.3.2.16.1731(long param_1,long param_2);
long * ifio_7fe3a.3b1a3eaa54584c817b8803caef413f5f0d4.3.2.16.1731(long param_1);
void ifio_343e2.1142b122077477692eae101cbd150d0c487.3.2.16.1731(long param_1,long param_2);
long ** ifio_cfae1.853909b17879a9220d94994211ec2e39537.3.2.16.1731(long **param_1);
undefined8 ifio_0d2d6.675527e2f095683512e3a744b268ce049d3.3.2.16.1731(long param_1);
void ifio_07bfc.eb52dc80b63cc258c604f6716ac7d3e4da0.3.2.16.1731(long param_1);
void ifio_36203.6ae1050e3c3fa34f85fc2be72098dd33a8f.3.2.16.1731(long param_1,ushort param_2,int param_3,int param_4);
void ifio_14fda.1980eefb19e04b97b220e84ac1f79347af9.3.2.16.1731(long param_1,int param_2,int param_3,int param_4);
uint ifio_8fe4b.34b96b22a972cccb798b7e0c0b52cecc661.3.2.16.1731(long param_1,ushort param_2,uint param_3,uint param_4);
undefined8 ifio_bf484.10adf66038f67d2bebdef5cd269658b1b81.3.2.16.1731(long *param_1);
undefined8 ifio_57741.58ae400a29e064693056c91c0b3de0bb156.3.2.16.1731(void);
void ifio_fba47.256505f75f437c8e3f93a9b41f11cde48e4.3.2.16.1731(long param_1,long param_2);
undefined8 ifio_3ef5a.35b3f13a16b50450ae72d08e758f5e3d948.3.2.16.1731(long **param_1);
undefined8 ifio_290ba.ac53c1f9b4dee62137ebdbe262185fe9f66.3.2.16.1731(long param_1,int *param_2);
void ifio_a209b.2e89f66e00d71400287ff00b4e550fb3f28.3.2.16.1731(long param_1);
void ifio_5d8ec.a1d13769552bbd7ff10086a4e6de52ab388.3.2.16.1731(long param_1,long param_2);
undefined8 ifio_03dd4.a9604c8b4d688a52cf83a1ed074042348f6.3.2.16.1731(long param_1,int *param_2);
ulong ifio_80d86.16da3cc7c619b641d551204dd4e16dadf7e.3.2.16.1731(long param_1);
undefined8 ifio_96ae0.27bdf1684b23802cd55a3d26ad90239db91.3.2.16.1731(long param_1,short param_2,char param_3);
void ifio_dd14f.3dfbb2096873116a3f8d3d988902a206d23.3.2.16.1731(long param_1);
void ifio_b0c92.cf695007ca0b0d881236a6db55eb6b31425.3.2.16.1731(long param_1,ushort param_2);
void ifio_d25cb.3d6f5bc5dfd435c2d6ac44bfbbab17f440c.3.2.16.1731(long param_1,long param_2);
void ifio_867d0.64985ad0ed1d376a838355bb93a2cd8a49a.3.2.16.1731(long param_1);
long ifio_3fd18.7d0bcb29286fed3024fd857b90c9ed8e82c.3.2.16.1731(long param_1,int param_2);
undefined4 ifio_1c194.f280f7cd1a3bbc17b5a400ef89cbf4e5443.3.2.16.1731(long *param_1);
void ifio_a74fd.ace28616c7ea46e78c29fd7acff4856dec0.3.2.16.1731(long param_1,ulong param_2,uint param_3);
ulong ifio_021e5.7aa1d77181c8d0dfe45e05efaa222accd82.3.2.16.1731(long *param_1);
void ifio_cf786.678870ac327516d950e7950c9e3e6d2b8cb.3.2.16.1731(long param_1);
void ifio_5c674.6f5e66209280346d0f86a016bb0073a069c.3.2.16.1731(long param_1);
void ifio_7134b.1d3c455fcc94e4cd7d27a12ef3618e31e51.3.2.16.1731(long *param_1);
bool ifio_fc11b.ba83225920939e27743257adf4d1b05c310.3.2.16.1731(long *param_1);
undefined4 ifio_10953.3943395e295bdd3da697651dcf192db9cb8.3.2.16.1731(long param_1);
ulong ifio_2856b.ac63627a5643a4d1108901df49089633d1d.3.2.16.1731(long param_1);
void ifio_df63b.f9a97869a78f083b75b3afd1ffed6382092.3.2.16.1731(long param_1);
uint ifio_9519e.2ae07940b9af2776ea25efedbb44abc0534.3.2.16.1731(long param_1);
uint ifio_84b08.598c6c97603c0ad2d6f231a9549ce12159b.3.2.16.1731(long param_1);
uint ifio_60e34.2ddc4175e083240cef5c83cff4e57501331.3.2.16.1731(long param_1);
uint ifio_9b55c.d1bd535f5e0713f71387c2fd4e61d16610e.3.2.16.1731(long param_1);
uint ifio_f3ce0.39bd7ef85ef817fceed3fe5f653134ddee5.3.2.16.1731(long param_1);
void ifio_755dd.fccc2c7bedc7e166106a12cb00adbc8baac.3.2.16.1731(long *param_1,int param_2);
undefined8 ifio_0018a.a3a0c5ca6d4e739478d0f222b0c08cb43b2.3.2.16.1731(long **param_1);
void ifio_78b61.346beeacbb190fe216c387932fcef9639fc.3.2.16.1731(long param_1,char param_2);
void ifio_05056.892ad5f48be91f0fe018b3ca56ea510a0cd.3.2.16.1731(long param_1,long param_2);
undefined8 kfio_fio_device_from_nand_dev(long param_1);
undefined8 kfio_dircache_get_vsl_callback_version(undefined4 *param_1);
undefined8 kfio_dircache_register_handlers(long param_1);
void kfio_dircache_unregister_handlers(void);
undefined8 ifio_de395.d1a685ac36f55570e5808c076d7dad8dcb1.3.2.16.1731(long param_1);
undefined8 ifio_bdf66.97169d9a4d1e9cf80297c3d7dd675208c9c.3.2.16.1731(long param_1,uint param_2,undefined8 param_3,char *param_4);
void FUN_001225d0(ushort param_1,long param_2,undefined8 param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6,undefined param_7);
undefined8 ifio_94f98.0b8163a2c03fda0252ebfa757bd776f0180.3.2.16.1731(long param_1);
void ifio_e0d5f.cc9ec0c5b14ead97d5e237619efdd8eb758.3.2.16.1731(long **param_1);
void ifio_908c1.33a116d2534295fe9b6ca67993effc80335.3.2.16.1731(long param_1,long param_2,char param_3);
undefined8 ifio_01c6d.13e389e0bc27c2cdc59c93072100a71dcfb.3.2.16.1731(long *param_1,uint param_2);
ulong ifio_7237d.320d87c055d1ed15891fe4e79dacbc10c8b.3.2.16.1731(long *param_1,undefined8 param_2,undefined8 param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6);
void ifio_543b1.69e70daab820067cc7b92d9818cdc70566a.3.2.16.1731(long param_1);
void ifio_0b721.9fe8f7da32e9403d1c404487860316f3da1.3.2.16.1731(long param_1);
undefined4 ifio_13a7f.633e2e1a4121d4b27a4c536c2fb4eebd030.3.2.16.1731(long *param_1,ushort param_2);
undefined4 ifio_628e1.6a52e9b1a45afdc5c7258271649435746d4.3.2.16.1731(long *param_1,ushort param_2,char param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6);
long ifio_a9d84.a3496613e0697e5adc3dcc044e967597c88.3.2.16.1731(long param_1);
void ifio_ab9c3.5e2e4e8c64362ae39bdaea5c66577eb223d.3.2.16.1731(long param_1,long param_2);
undefined8 ifio_3610c.c77a76b24c395a447afd5d14c5577f3a634.3.2.16.1731(long param_1,ushort param_2,undefined4 param_3);
void ifio_ae1a0.8f5b5248a24da674a0c3e614ec92cdfa68d.3.2.16.1731(long *param_1,long param_2);
void ifio_497ba.0c879e7128d7e0215dfd7bf06df2d36b28f.3.2.16.1731(long param_1,ulong param_2);
ulong ifio_ff0be.1d3ef295e1e0ec37dad5ee4573440425421.3.2.16.1731(long param_1);
void ifio_a50bd.cfce1ba29b013cb2de952323f0e7541965b.3.2.16.1731(long param_1);
void ifio_990d8.ead16b73b646818f19d2a0ab1af630dc722.3.2.16.1731(long param_1);
bool ifio_a3072.d5260a764badee41d0892039c4794870812.3.2.16.1731(long param_1);
void ifio_e21f2.72d5871a42a1c3626f7322b248a6318050a.3.2.16.1731(long *param_1,ushort param_2);
ulong ifio_d4361.125f79b95a1f085288eaf608bb8b843331f.3.2.16.1731(long param_1);
ulong ifio_c92ed.4ce2f933901c4f582fbb41ddef1730d8795.3.2.16.1731(long param_1);
bool ifio_e7856.4db0d91510273f47e4e1d5d817df3f70e20.3.2.16.1731(long param_1);
ulong ifio_9bb1c.5180ff4071a88fac7d6fa3d475b5c410131.3.2.16.1731(long param_1);
long * ifio_2c10a.7d58df51f2299f4e45ac6e12eada4c8a8ef.3.2.16.1731(long *param_1,undefined *param_2);
void ifio_422ad.7e604efaaddc8f1396365af25cc1f732bc8.3.2.16.1731(long param_1,long param_2);
int ifio_fb0be.f99990e20cd9fd7ca5cfa8c80a4cf3201c2.3.2.16.1731(long **param_1,uint param_2,ushort param_3);
int ifio_d774c.878dc7e854d067edef55406196bca8778a0.3.2.16.1731(long **param_1,ushort param_2);
int ifio_cdfbb.818dbdb6676f018a2e33930d9b22497e9e8.3.2.16.1731(long param_1);
void ifio_642ab.dd56c1597bf218092ef90eacbaecd6d7880.3.2.16.1731(long param_1);
undefined4 ifio_0b493.38b8afe6e50541b8a654ec30df0486d2c1f.3.2.16.1731(long param_1,ulong param_2);
uint ifio_fb5db.fb3e7e27fe85c53675527309c7f89b19650.3.2.16.1731(long param_1);
uint ifio_9d47e.540c94fd26771bda8f90b84b8d7f3a8ee1b.3.2.16.1731(long param_1);
undefined8 ifio_e0c3e.be46a50a27daf1abc846dc54540a26b7ff1.3.2.16.1731(long param_1,int param_2);
void FUN_00124090(ushort param_1,long param_2,undefined8 param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6,undefined param_7);
void FUN_001240f0(ushort *param_1);
void FUN_00124270(long param_1);
void ifio_c0166.d04f2cf103bd35036b2043f66b95ca15c28.3.2.16.1731(long *param_1,ushort param_2);
undefined8 ifio_c0f79.fc503b2c071ac6c5ea8dd8e3d28eefe3ce9.3.2.16.1731(long param_1,long *param_2,short param_3);
undefined8 ifio_b487c.8711ef1ba8f0b93a537402d89f0bea2dc61.3.2.16.1731(undefined8 param_1,long *param_2);
void ifio_7dd4e.cbc0178dc9ba6063ee9874fe65749cfa43c.3.2.16.1731(long param_1);
undefined * ifio_cc6f3.a9c6081fc1a7fe9e4683195579dfe28c3ae.3.2.16.1731(void);
void ifio_eea04.e6662433082816316afd8024bb45f344194.3.2.16.1731(void);
void ifio_61890.7171812f52a93ed285ddcf7dea12da5200a.3.2.16.1731(long param_1);
void ifio_a22db.0f2b76a15fc7a43313e5ff897d5c5a6facc.3.2.16.1731(long param_1);
undefined8 ifio_f2566.8bcf7bf23157eb955897c1ba91d2bd27ccb.3.2.16.1731(long param_1,undefined *param_2,undefined8 *param_3,int *param_4);
void ifio_bb451.9d4d127c5f8ff494f8768cdb0444446b255.3.2.16.1731(long param_1);
void ifio_79bdb.0d852078b373028122235577d32633522a6.3.2.16.1731(long param_1,uint param_2,int param_3);
ulong ifio_947ad.2e1b9a963ee97dcb4ccf184d528bf5a391f.3.2.16.1731(ushort *param_1,uint param_2,byte *param_3,int param_4);
void ifio_61804.29ddeff27ece1f0f5b1500ead9897a41d4e.3.2.16.1731(ushort *param_1,uint param_2,uint param_3,int param_4,int param_5);
undefined8 ifio_28624.f1fd1e6e0a571f661d586d1f353bf24dcb2.3.2.16.1731(long param_1,ushort *param_2,uint param_3,uint param_4);
undefined8 ifio_e4d19.7c436606d9fa7b61ecb348926942506c121.3.2.16.1731(long param_1,ushort *param_2,int param_3);
undefined8 ifio_f7dbf.f828779f73381a991126a57530c8f9a538d.3.2.16.1731(ushort *param_1);
int ifio_17602.e65cb058ad57a458de0227a07c8edc2d30d.3.2.16.1731(ushort *param_1,int param_2,int param_3,int param_4,int param_5);
ulong ifio_c522c.bfafca5f9e30306f6aba1785e4936b9c92e.3.2.16.1731(long param_1);
void ifio_4e31d.71a468f9fe14a816b1230eff960aa0ee33f.3.2.16.1731(long param_1,undefined8 param_2,undefined4 param_3);
void FUN_00125fc0(long *param_1);
void ifio_c71be.047db03043b5da27b45b3ed00211f830838.3.2.16.1731(long *param_1);
void ifio_462e2.2b0aac3642198649bfab4dcfb3e530f3a82.3.2.16.1731(long param_1);
undefined8 ifio_e1895.56cd427dd2e71cb7ff2c5f00716debea45f.3.2.16.1731(ushort *param_1,ushort param_2);
undefined8 ifio_5c8a3.d52d7ae38fe6da06d507ba99f82a50ade26.3.2.16.1731(ushort *param_1,long *param_2);
void ifio_5c758.2b60fdd16b8be6a4e77277d2eb431313c78.3.2.16.1731(long param_1);
void ifio_85dba.0ac3f67fef8c9b37c3438258f2b1681c7cc.3.2.16.1731(undefined2 *param_1);
void ifio_a7cca.1ea7c4ae8761abf4f9fd1cb500bbf2de426.3.2.16.1731(ushort *param_1);
undefined8 ifio_d838e.beb0c7e6bc48f6823d158232eb95367ecc8.3.2.16.1731(ushort *param_1);
undefined4 ifio_45a71.ae40827bea47f631141f324aae758814db7.3.2.16.1731(long param_1);
undefined8 ifio_d6b99.ca6ed183e8e3e2c507c7b1dc3cced6eac6a.3.2.16.1731(long param_1);
void ifio_11b75.d204f16c08d507ff67314388f5be2c12c63.3.2.16.1731(long param_1);
uint ifio_022aa.f61ef6acb78443cb1bf63acab379c63716b.3.2.16.1731(long param_1);
ulong ifio_cc490.dfedbdc2c99646d35bb3638dde7ca3df43d.3.2.16.1731(long *param_1,int param_2);
void ifio_92fc0.066f866d42161efb071b8d168f6d4f4803e.3.2.16.1731(long param_1,int param_2);
void ifio_934af.e20f8a027f839e82a1e72b8c3acb2392714.3.2.16.1731(long param_1);
undefined8 ifio_48c38.6c6d34d420a70b42fadaf4a32340f801a3c.3.2.16.1731(long param_1,undefined8 param_2,long param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6);
undefined8 ifio_ef4ce.f5b42cff88fe0ee9cd0ea4d1bc43fa08248.3.2.16.1731(long *param_1,uint param_2,long param_3,long param_4);
undefined8 ifio_1e27f.4a2fe1f394bb6ed08e027b24aba462446ac.3.2.16.1731(long param_1,uint param_2,long param_3,long param_4);
void ifio_e7e23.ce4f6c7784665b48d94e1debb53e710d17d.3.2.16.1731(undefined *param_1,uint param_2);
void FUN_0012b620(long **param_1,undefined8 param_2,undefined8 param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6);
void FUN_0012b700(ushort param_1,long param_2,undefined8 param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6,undefined param_7);
int FUN_0012b760(long **param_1);
void FUN_0012bad0(long **param_1);
undefined8 ifio_0609d.72bb6a1f0b7fe5c7310cb96ca33e706e097.3.2.16.1731(long **param_1);
undefined8 ifio_05a67.90add67b08751d853402d2dd7981042254e.3.2.16.1731(long *param_1,undefined8 param_2,undefined8 param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6);
void ifio_33864.a61715aa24ef4bbdfe6ee29af19dd869ddd.3.2.16.1731(long param_1,undefined8 param_2,undefined8 param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6);
void ifio_6d9c8.ac9dd392c24dbaaf518cfad8f7abb937dec.3.2.16.1731(long param_1);
void ifio_1eebb.34bfe53dbbb10a01320128433f447d090f6.3.2.16.1731(long param_1);
int ifio_b6806.dd9ae1c2de2430068cc4ce5e706efa2f26c.3.2.16.1731(long param_1,ulong param_2,int *param_3);
ulong ifio_b312c.cc393201a9f9d6a33646bb8f2959d8e0280.3.2.16.1731(long param_1,ulong *param_2);
void fio_count_sectors_inuse(long param_1,ulong param_2,long param_3,undefined8 *param_4);
ulong ifio_11513.377f55cbaed6036e9c0a8a2d9fac7057640.3.2.16.1731(undefined8 param_1,long param_2,ulong param_3,long param_4,uint param_5,uint *param_6);
ulong ifio_f589a.c830ef7141c8756d494d97b8b7d093c1cfd.3.2.16.1731(int *param_1,ulong param_2,ulong param_3,long *param_4);
void ifio_c3820.fc176463d0658def779695324668d06de3f.3.2.16.1731(int *param_1,ulong *param_2);
void ifio_1dde9.231aa0734e3baa8a008df9efa9561d41f6e.3.2.16.1731(int *param_1,ulong *param_2,long param_3);
undefined8 ifio_76077.b28e6690f5899e2d2c0e0f4c1cf78ff99f7.3.2.16.1731(uint *param_1,ulong param_2);
void ifio_5b416.e8d8e4d51e04d84383ac1aee2859803234f.3.2.16.1731(uint *param_1);
void ifio_8df23.5841b92c33a683151729df762dd2cc1e217.3.2.16.1731(long param_1,long *param_2);
void ifio_688f5.24d74b229560b0cbd79eb5b4fc55f427147.3.2.16.1731(long param_1,long *param_2);
void ifio_77e34.f9f6832f8d32710fdd91b25ae8f0f9ff47a.3.2.16.1731(long param_1,ulong *param_2,long param_3);
void ifio_bbc82.29e519d9ba6048226c0701701ed218ecaef.3.2.16.1731(long param_1,long param_2);
undefined8 FUN_0012d840(long param_1,undefined8 param_2,long param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6);
undefined8 FUN_0012d900(long *param_1,undefined8 param_2,long param_3);
undefined8 FUN_0012dbf0(long param_1,undefined8 param_2,long param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6);
undefined8 FUN_0012dc20(long param_1,undefined8 param_2,long param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6);
undefined8 FUN_0012dc50(long param_1,undefined8 param_2,long param_3);
undefined8 FUN_0012dd60(long param_1,undefined8 param_2,long param_3);
undefined8 FUN_0012ddf0(long param_1,undefined8 param_2,long param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6);
void ifio_9e8d0.48ac345233cf9b8d1906507a99571e14acc.3.2.16.1731(void);
void ifio_6c899.87d61630f7ac42daac4986aa0ce07ae16a2.3.2.16.1731(void);
ulong ifio_3083c.b2b1eeec83534d7e66fc9d1b210cc782b8c.3.2.16.1731(long param_1,long param_2);
undefined8 ifio_49bdc.b6a3be9cf8670b9b26eb1b73328fd8776bf.3.2.16.1731(long param_1);
ulong ifio_a5f42.24253bfb729c91277a79662d48253092584.3.2.16.1731(long param_1);
undefined8 ifio_c04a8.82127b119e4b58c1eadac392f134484ee8f.3.2.16.1731(long param_1);
ulong ifio_ec9dc.82748286d9f2f58fa61529a857ac8f6e899.3.2.16.1731(long param_1);
undefined8 ifio_a5c0e.37d52e99bb83dc5bbddca6451f81148758f.3.2.16.1731(long param_1);
undefined8 FUN_0012e550(long param_1,uint param_2,long param_3,long param_4);
ulong FUN_0012e6f0(long param_1,uint param_2,long param_3,long param_4);
ulong FUN_0012e780(long param_1,uint param_2,long param_3,long param_4);
ulong FUN_0012e810(long param_1,uint param_2,long param_3,long param_4);
ulong FUN_0012e8a0(long param_1,uint param_2,long param_3,long param_4);
ulong FUN_0012e930(long *param_1,ulong param_2,long param_3,long param_4);
ulong FUN_0012ea30(long *param_1,ulong param_2,long param_3,long param_4);
undefined8 FUN_0012eb30(long param_1,undefined8 param_2,long param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6);
undefined8 FUN_0012ec60(long param_1,undefined8 param_2,long param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6);
ulong FUN_0012eca0(long param_1,uint param_2,long param_3,long param_4);
undefined8 FUN_0012ed70(long param_1,uint param_2,long param_3,long param_4);
ulong ifio_f2763.4ee03f4ae7ec4d5a30becd57a47eebf4821.3.2.16.1731(long param_1);
undefined8 ifio_47135.736d35d17a8005db82430d02ba4b6dc3708.3.2.16.1731(long param_1);
void FUN_0012f430(void);
void FUN_0012f630(void);
undefined8 ifio_37bbd.1cc167bbfde28817dfd4b46d157e60e49e5.3.2.16.1731(void);
undefined * ifio_443f2.e35c2e130df329556101ccc38f4de44ac04.3.2.16.1731(void);
undefined * ifio_894c2.2fa1d179e65baf67ab805fbf3a9c24ae38c.3.2.16.1731(void);
undefined8 FUN_0012f8c0(undefined8 param_1,uint *param_2,long param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6);
undefined * ifio_6c999.b496283a6d2eb18743c026fcce295c8c664.3.2.16.1731(void);
undefined8 init_fio_blk(void);
void cleanup_fio_blk(void);
void ifio_673ef.a406e1cb9c12247e10d9e2739569e6926af.3.2.16.1731(long param_1);
long ifio_8fc48.625d084b2d02a25ca09362e89ea177a44f4.3.2.16.1731(long param_1);
int ifio_82837.a90a8f4701bf60eaa2f77cf8a069179dd8d.3.2.16.1731(long param_1);
ulong ifio_b417d.046d7540503d3c98acb14a5f1b5b614b6c8.3.2.16.1731(long param_1,long param_2,long param_3);
undefined8 ifio_eeb18.05c89ebb1951f9a8a0244620e1dc61640d8.3.2.16.1731(long param_1,ulong param_2,char param_3);
void ifio_67261.fdecf72623035e2a8dfd5eb2641e2340eaa.3.2.16.1731(long param_1,long param_2,long param_3);
ulong ifio_98981.0c55850abddb730f8112f040a40870b8434.3.2.16.1731(long **param_1,long *param_2,undefined8 param_3,uint param_4,char param_5);
int ifio_c62d3.fd06bb89a822bdb694f099fb9957c7fb0c4.3.2.16.1731(long *param_1);
uint FUN_00130b10(long *param_1,long param_2,undefined *param_3);
void FUN_00130d80(long param_1);
undefined8 ifio_77cde.630566014bb1b977fbcc7b0188b051409c9.3.2.16.1731(long *param_1);
void ifio_1ff2e.262da71b66f0af4de444647ff33f1ee8ac0.3.2.16.1731(long param_1,long **param_2);
undefined8 ifio_6ff64.0ff170d88338d1683cd9d2dffdb88483c08.3.2.16.1731(long **param_1);
void ifio_44bdc.f96915ed0d3d8b06a8115c09843169f9676.3.2.16.1731(long param_1,ushort param_2);
int ifio_334b6.ce717199a1f54b15deffd09f7ed9ec82405.3.2.16.1731(long *param_1,ushort param_2);
int ifio_a0459.faa3cc4943bf84f50ed307c478f7ea78f21.3.2.16.1731(long *param_1,char param_2);
void ifio_f83df.092d603cb3bbbab564adc64c8c138b092d5.3.2.16.1731(ushort *param_1);
void ifio_94689.54b35e7a19dd7863217e803225524e63899.3.2.16.1731(ushort *param_1);
int ifio_1b2be.4176732f58eeee1ba11a9009738735a0a6a.3.2.16.1731(ushort *param_1,ushort param_2);
ulong ifio_d6b67.a3e1dec782277888d25946cba1fa2ac3869.3.2.16.1731(ushort *param_1,ulong param_2,ulong param_3,uint param_4);
ulong ifio_965f6.7834d67995025d028e974a881c3fbb7b3d4.3.2.16.1731(ushort *param_1,int param_2);
uint ifio_68ac8.80aaad1073a18eff1c2bec9b4d44a81daeb.3.2.16.1731(long *param_1,long param_2,ushort param_3);
void ifio_e05d3.1088fd3f9903c1a54bd3a4ee085c7e99386.3.2.16.1731(long *param_1);
void ifio_f0d19.bbf53cadc42e984534e43f33ede022e7c4b.3.2.16.1731(long *param_1,ulong param_2,char param_3);
ulong ifio_507e1.0a2bf9c492d1ea2582ebb63a6bbee5ef33b.3.2.16.1731(long param_1,long param_2);
void ifio_345d7.473381340536ebaf1243ca3312d73a88870.3.2.16.1731(long **param_1,ushort param_2);
bool ifio_168b5.fba94fe2cef3760d372a3843b924ad4f70b.3.2.16.1731(long **param_1,uint param_2);
undefined8 ifio_cf910.8707c02d5725d550098087b5b2067f57ca4.3.2.16.1731(undefined2 *param_1,undefined8 param_2,undefined8 param_3);
ulong ifio_a9176.1a931133754e60454f0e38305c41e03f958.3.2.16.1731(long **param_1);
undefined8 ifio_39afc.4b90f93c994ed7006904466cf8dd6bb80a5.3.2.16.1731(long param_1,char *param_2,ushort param_3,undefined4 *param_4,char **param_5);
int ifio_f0741.3a441342369244f82f6a0d804da952e39d3.3.2.16.1731(long *param_1,long param_2,ushort param_3,char **param_4,char param_5);
int ifio_2d9bb.2b936195511394659485d7f5e0cc0248f84.3.2.16.1731(ushort *param_1,undefined8 param_2);
ulong ifio_ce375.736b1279766f8fcc85f1d9a1422eb522dd1.3.2.16.1731(ushort *param_1,undefined8 param_2,undefined8 param_3);
int ifio_fc320.3d55e7ae9d927956a45f5f762700e2471c5.3.2.16.1731(ushort *param_1,ulong param_2,undefined8 param_3,char param_4);
ulong FUN_00134450(ushort *param_1,long param_2,undefined8 param_3);
int FUN_001345f0(long **param_1,uint *param_2,byte *param_3,char *param_4);
ulong ifio_e4bba.e77b4876b61cc1f5a62a96afe635cb8bc25.3.2.16.1731(long *param_1,ulong param_2,ulong param_3,uint param_4);
void ifio_eb605.52497324d2002464921fdaeab4bc6615db7.3.2.16.1731(long param_1);
ulong ifio_f79a1.4ee864b3847363d661dbc7b5a496589787b.3.2.16.1731(long param_1,ulong param_2);
undefined8 FUN_00136490(long *param_1,uint param_2,uint param_3);
void FUN_00136690(long param_1);
ulong FUN_001367b0(ushort *param_1,long *param_2,ulong param_3,byte param_4,ulong param_5,uint param_6);
ulong ifio_5d44a.a4e0f88d74ab3d4eb0d5869dba527bf1fd2.3.2.16.1731(ushort *param_1,ushort param_2,undefined8 param_3,char param_4);
ulong ifio_1048d.c9b2e70336eef04967d5a83861d0d5e4be4.3.2.16.1731(long param_1,long **param_2,long param_3,ushort param_4);
long ifio_b9b9d.d89aeef836a95db4f331215c8bf24f588ab.3.2.16.1731(long param_1,undefined8 param_2);
int ifio_2ae94.73c2c5dd9f08c8b90df30b27f0f2c1e0a2e.3.2.16.1731(long *param_1);
void ifio_f8f7e.d59bda29b935cb0e0920375d94538600a23.3.2.16.1731(long param_1);
ulong ifio_9aa00.d915a3cd65dbd267ad02b4a8df33d18b5e6.3.2.16.1731(long *param_1,int *param_2,undefined8 param_3,uint **param_4);
int ifio_bcafb.eeb4816438ed800a5a91b6c4f70c25c4804.3.2.16.1731(long param_1);
void ifio_c77d6.0e65f061ba968ce77483e0bd6c0d30a0712.3.2.16.1731(long param_1);
undefined4 ifio_40853.a61b0497118aa099f83c56d55cfe222c038.3.2.16.1731(undefined4 *param_1);
void ifio_f3551.5fb989026ac06fbe3497102e2be627108b9.3.2.16.1731(long param_1,undefined8 *param_2);
undefined8 ifio_70b18.d10be62e1fd622702ea6f9574ed4c2e8c6a.3.2.16.1731(long param_1,int param_2,int **param_3);
uint ifio_487cb.d734a34e4f6d5c0376c76138b9ab080d03f.3.2.16.1731(long param_1,undefined8 param_2,long *param_3);
undefined8 ifio_5176e.82038fcab88e3164fd55b23d54e294f4d44.3.2.16.1731(void);
undefined8 ifio_32d98.2b3ae1d2b6fb7755fadd53aa29990ccb694.3.2.16.1731(long param_1,undefined4 *param_2,ulong param_3);
undefined8 ifio_fa6d9.76c27d5fbd7d277ea7c8eb2eaf82ab1dd1c.3.2.16.1731(long param_1,ulong *param_2,ulong *param_3);
undefined8 ifio_21789.10f721dc012c5a6d6a68282f3528b7bd299.3.2.16.1731(long param_1,ulong *param_2,ulong *param_3);
undefined8 ifio_f2890.07d29321456a74788b35b997c9f925275ec.3.2.16.1731(long param_1,ulong param_2,long param_3,long *param_4);
undefined8 ifio_481ab.38c0429287bb273b5294dff2f35aa3b33d3.3.2.16.1731(long param_1,ulong param_2,long param_3,long param_4,char param_5);
undefined8 ifio_e9493.eb6ee8bb4379bd3b091ef4d56d8ec4e1f27.3.2.16.1731(long param_1,undefined8 *param_2,undefined8 *param_3,undefined8 *param_4);
undefined8 ifio_9c8ec.082eb8ce96b0770de56b0eab6d89b5ee25e.3.2.16.1731(long param_1,ulong param_2,long param_3,undefined8 *param_4,undefined8 *param_5,undefined8 *param_6);
bool ifio_b91c2.ac543505e30c30b6ff152c2db2a8b737242.3.2.16.1731(long param_1,ulong param_2);
int ifio_75587.1f1e6530f6be0b4f1f0f93469a6eaa9a501.3.2.16.1731(long *param_1,undefined4 *param_2);
void ifio_ef73c.1002c953dd0fa7003d8df4770bc503934f6.3.2.16.1731(undefined4 *param_1,long param_2,int param_3);
undefined8 ifio_79a7d.26def278a76979e45b75061b5beccd65194.3.2.16.1731(long param_1,long param_2);
ulong ifio_10ce5.b04023782cc1c7de784c17e71cd2b6dc0bb.3.2.16.1731(long param_1,uint **param_2,long param_3);
undefined8 ifio_7dfc1.a39790bf99108d780a651b3a54c3f96a74b.3.2.16.1731(long param_1);
int ifio_3a291.a7da4cee684cc802b6acbb3af00ea6a38af.3.2.16.1731(long param_1);
int ifio_a89f1.13a5abe15a7253af335be233320e24310a7.3.2.16.1731(long param_1);
int ifio_22d9b.8d2634391a38dd08b569ed7ac9dd8a63faf.3.2.16.1731(long param_1);
int ifio_4dabe.807fcc0bea49af9fa2e1a8f39a2eab08424.3.2.16.1731(long *param_1);
ulong FUN_00139e00(undefined4 *param_1,uint param_2,long param_3,long param_4);
ulong ifio_fadc3.377599a6cd8380baf929970796fb6398824.3.2.16.1731(long param_1,long param_2);
undefined8 ifio_c2cfe.15bd34a29407253538d408dd9289a95f25d.3.2.16.1731(undefined8 param_1,long param_2);
ulong ifio_66071.e84653add2908c7555f0e3f1c37a1664e47.3.2.16.1731(long param_1);
undefined8 ifio_b996c.f07c1afc34c569d27ef50534f2313531aee.3.2.16.1731(long param_1);
undefined8 FUN_0013a080(long param_1,long param_2);
void ifio_6fa68.c7097bde06719c31bf1b7a6ad46ed4b2890.3.2.16.1731(undefined8 param_1);
void ifio_eea7c.6fd2ef96cc9a8c80e985ccc3a2751da9b5b.3.2.16.1731(long param_1);
int ifio_98eb2.3e2ad70185b7760c2e33d54f39421aeac40.3.2.16.1731(long param_1);
void ifio_b1760.51deb95af7bcac43ff607265fea6779f673.3.2.16.1731(long param_1);
void fio_bio_posted(long param_1);
uint ifio_845ec.8cdeec38352de11881f56803969e5371ff3.3.2.16.1731(long param_1,int param_2);
undefined8 ifio_7505d.a0ea8b90f5be76b75aeffbc3d24cc91a3a6.3.2.16.1731(long param_1,ushort param_2,ulong param_3,uint *param_4);
undefined8 ifio_d7b14.ab2a21eafc135821378976173ad8aa39188.3.2.16.1731(long param_1,ushort *param_2,uint param_3,long param_4,long param_5,char param_6);
void ifio_4b663.817bf1567f947c7a13dede9f825d319ee62.3.2.16.1731(long param_1);
void ifio_1a4e4.26e9ea4de4692f6853a2bc9ce3de173e7e5.3.2.16.1731(long param_1);
void ifio_d91fb.4cbf6b7a93d9bd228ff12e36e626d871cc5.3.2.16.1731(long param_1);
long ifio_b5d5b.e21bdf83d9d42d256f8640c805442b31902.3.2.16.1731(long param_1);
long ifio_d9762.8f9542e77a572c0c26c820f7eb602f98330.3.2.16.1731(long param_1);
long ** ifio_f1d1c.35cc0ada7706209dbb91a027da3cbc7848b.3.2.16.1731(long *param_1,ulong param_2);
long * ifio_15dad.e69f169524b169caf74fa8eacf597622016.3.2.16.1731(long param_1,ulong param_2,long **param_3);
void ifio_d9061.a5a6ee82758a4deac90350359fbc3efc97c.3.2.16.1731(long param_1);
void ifio_d0cdc.d0e0b9ee1de2b71a546a5b3f5f710fb87a9.3.2.16.1731(long *param_1,undefined8 param_2);
void ifio_e946f.382ddcd38e5fa8bc4ec81b7ba9ba51b186c.3.2.16.1731(long **param_1);
void ifio_2033f.7d22ab066ae91eae2290e506722f67b617e.3.2.16.1731(void);
void ifio_1d55b.6706131828ab80c3fcc1ae9412ae71bdc0c.3.2.16.1731(long param_1);
undefined8 ifio_d27e3.13f12dbbcf338dd14bc56d05a692f36abcb.3.2.16.1731(long param_1);
int ifio_d1d0d.eef91034f11fe1c21fed5bd2855a4b322d4.3.2.16.1731(void);
void ifio_dcb32.6a2bb0f4d496cb1171a834bd79e010b85ec.3.2.16.1731(void);
void FUN_0013c760(long param_1);
void ifio_20504.30ddb3dcd2bf8640eb3e7fe0aad76aa96ec.3.2.16.1731(long param_1,undefined8 *param_2);
undefined8 ifio_72f71.2c2bd356225dd8edb053876156fabb98276.3.2.16.1731(long param_1,long param_2,ulong param_3);
void ifio_2e83a.9e1f9167e713698c90994a732d06a656384.3.2.16.1731(long param_1,ulong *param_2,long param_3,int param_4);
void ifio_8411d.55989013790734c146adda13ad41192da4a.3.2.16.1731(long param_1,long param_2);
void ifio_96fbf.1137c02633cfa1d46edd778f873a90841b6.3.2.16.1731(long param_1);
ulong ifio_95f66.9d0b84bd290b9cf8ef0c94f14d021625304.3.2.16.1731(ulong param_1);
void ifio_5afd6.c5d26f06de0574b5b17a88c4042c8d59b97.3.2.16.1731(long param_1);
void ifio_22d18.0ebf661443955ab78fce90ad40ce09713a4.3.2.16.1731(long param_1,uint param_2,int param_3);
void ifio_e8fd2.2551ea4ef45387bc6aebc705d32aa78c964.3.2.16.1731(long param_1,long param_2);
ulong ifio_fdad8.a514d60255434434bb9e5f3fefe213fe20d.3.2.16.1731(long param_1,ulong param_2);
void ifio_85321.f2842516e03d55dc67ab34244b8f311bcfc.3.2.16.1731(undefined4 *param_1,short param_2,short param_3);
int ifio_ba23d.96fc7f682095c340b331be1c590cd61ae40.3.2.16.1731(int param_1,byte param_2);
void ifio_547ea.8713a2d06ba63315fc0051e99d1cd736a41.3.2.16.1731(int *param_1,int param_2);
void ifio_1a389.ca609a88f253ebbfb237e2bf4cdbb3a7b3e.3.2.16.1731(long *param_1,ulong param_2,uint param_3);
void ifio_48890.e6725dc6bf6f48b7eefa6a75b5f4dc9d077.3.2.16.1731(long *param_1);
ulong ifio_7bf10.d67ef2156b508ff74993d8dba622de10582.3.2.16.1731(long *param_1,long param_2);
ulong ifio_9a22d.20c6860ad42fa4586c2a17b3e184e903ad6.3.2.16.1731(long param_1);
void ifio_0d85a.e6d985b4e58c9c72abf2fec2045828f7c9f.3.2.16.1731(long *param_1);
void ifio_98ab5.866d5721768feab22ff0da938714e0f2d71.3.2.16.1731(long *param_1,long param_2,long param_3,long param_4,long param_5);
void ifio_9f489.fd40ae30a7c309f52cc1e15ba9613cf1ad3.3.2.16.1731(long *param_1,long param_2,long param_3);
undefined8 ifio_5a211.1b91cb8b614b2653213477c32ebfc3ddc3b.3.2.16.1731(long param_1,byte param_2,byte param_3,int param_4,byte *param_5);
undefined8 ifio_0c6c2.d81a9f953eb1e008bf0fd270469de02157a.3.2.16.1731(long param_1,byte param_2,byte param_3,int param_4,byte *param_5);
undefined8 ifio_0da46.5a0238053cb0cc877ed593d2e5ef4f5bf43.3.2.16.1731(long param_1,byte param_2,byte param_3,byte *param_4);
undefined8 ifio_cfe71.ba7e3f35b1fb6816e672831387fbcb342f0.3.2.16.1731(long param_1,byte param_2,undefined param_3,undefined param_4);
uint ifio_c1d34.fbc9d893e4f291999c3d2b6d6196cee0dfa.3.2.16.1731(long param_1,uint param_2);
undefined8 ifio_333a5.e243937d6b52cf62e5e196698d96f2fb725.3.2.16.1731(long param_1);
byte ifio_2ac65.f2de226a521eb1fc90907bfdaea4c80a691.3.2.16.1731(long param_1,byte param_2,byte param_3);
undefined8 ifio_cadc7.bb212fbae9e36b37fe57c8abbae483eb69a.3.2.16.1731(long param_1,byte param_2,byte param_3,int param_4,byte *param_5);
undefined8 ifio_be92e.70c2229548cf2fdf192628dde98fec58d7a.3.2.16.1731(byte param_1,char param_2,uint param_3,ulong param_4,long param_5,undefined8 param_6);
undefined8 ifio_5f845.b4ad1cc7414759e4f9c88fa81e0e5eb4ead.3.2.16.1731(long param_1,undefined8 param_2,long param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6);
ulong ifio_aae21.e84be5710b10c52af53b3dbe735d812f471.3.2.16.1731(long param_1);
undefined ifio_1c65f.6340f05c7502f6eaa2ad151d166791f2c97.3.2.16.1731(long param_1,ushort param_2);
void ifio_862a3.4150a9f48ce7249a3610a761bc3210db7f3.3.2.16.1731(long param_1,int param_2);
undefined8 ifio_8d6f9.6a35a7d20d755afc00443e1003ac0c1906f.3.2.16.1731(long param_1);
ulong ifio_f2e64.9f7dea1415a994bb60bb074b299a5c21994.3.2.16.1731(long param_1,undefined8 param_2,long param_3);
undefined8 ifio_17e57.33bee395f5bb6f8fff776a976f6e4cef1e5.3.2.16.1731(long param_1,int *param_2);
undefined8 ifio_89439.e9412ae3a703ee26ca91fcbb7717b9348d0.3.2.16.1731(long param_1,undefined *param_2);
void ifio_b0f3d.ce23010962e4b571ba143811dc4d745377c.3.2.16.1731(long param_1);
void ifio_19b9b.6a0211d1ec6905d6f5e3f580a4aa0c92781.3.2.16.1731(long param_1);
void ifio_805bc.43086b151beb047eca75d89f20b9d30614c.3.2.16.1731(long param_1,undefined8 param_2,char *param_3);
uint ifio_56902.e4727435a1cd94a73ccaa9afc74db881065.3.2.16.1731(long param_1,char *param_2,undefined8 param_3);
int ifio_6cc94.ad1bb35854b8fe9d4a3108a40224ef93208.3.2.16.1731(long param_1);
void FUN_0013ebb0(int param_1,int param_2);
undefined8 FUN_0013ed10(int *param_1);
void ifio_eb70c.c9ec8966ac19fd288820065984644b3f231.3.2.16.1731(long **param_1);
void ifio_5db12.6ec71332a9edce3b69736861a71c349ce14.3.2.16.1731(long param_1,long param_2,undefined8 param_3,undefined4 param_4);
void ifio_6c998.97314c53f448c599a5e2f25a1bf4318e565.3.2.16.1731(void);
ulong ifio_627cd.6b1963bc71a1db57d6590d9aca8918487d4.3.2.16.1731(void);
void ifio_9bf46.a9404129b1b054c932dc046ce5850d864fd.3.2.16.1731(byte *param_1,short param_2);
uint ifio_ebb6c.9a30b58a519dfe45d05958200d0de27ac00.3.2.16.1731(uint param_1,byte *param_2,short param_3);
undefined8 ifio_9a7ba.71b3a623bb317b48185178a36c30a8641a7.3.2.16.1731(void);
void ifio_309ce.ba165d1cc7ccd891a7835696f0cde30814a.3.2.16.1731(long param_1,undefined8 param_2,uint param_3,int param_4,uint param_5);
void ifio_05c2e.ddade9d3214dfcad812a67cc57427c9711e.3.2.16.1731(long **param_1,ulong param_2);
void ifio_18d70.8de25795517d4643bd9a6db4cbb99eaa6ad.3.2.16.1731(long **param_1);
void ifio_a473a.76ddd6af6fbd98674a3f25ce3c7e982516e.3.2.16.1731(long param_1);
void ifio_cab00.b62e5afd2dee174fd649a6bc07ed537b200.3.2.16.1731(long param_1);
void ifio_0f50b.510647a9be57193ed04464cdc3bb1a477f7.3.2.16.1731(long param_1);
void ifio_76b18.c2ba4f89d03ef0f69290cf21ad91a5e646f.3.2.16.1731(long param_1);
void ifio_79a6a.c834641b8a3ae2c1ee1738188d156a8a2e1.3.2.16.1731(long param_1);
undefined8 ifio_ac3a1.dad8d3927de69401206115e0be7d12addc1.3.2.16.1731(long param_1);
void ifio_d8b72.62d7f5723cb0548a632900bffb5fc53f964.3.2.16.1731(long param_1,undefined8 param_2,long param_3);
ulong ifio_f8201.b5afb11a8801c57234f02ea976d9c0fd5c7.3.2.16.1731(long param_1,int *param_2,uint param_3,long param_4);
void ifio_6d12c.3313ce9a46fcf34f68ef43dc8b4bded739e.3.2.16.1731(long param_1);
undefined8 ifio_5c3fe.7fb9d42e7b71b3e7034ea42c30777ae0677.3.2.16.1731(long param_1,long *param_2,uint param_3);
void ifio_0f69f.c632269ccac72cd2d30012f32c8fab514d7.3.2.16.1731(long param_1,long param_2);
undefined ** ifio_1eaa5.94bdc0948c72db604f690e5312597cf48bd.3.2.16.1731(long param_1);
void ifio_58be6.c6a2269608b1e009f9845833cc303ecabaf.3.2.16.1731(long param_1);
void ifio_6309c.339d2718bfb5091b1eec78847eabd1e86f7.3.2.16.1731(long param_1,ulong param_2);
void ifio_d5420.71cd759ad5a4aa2e78304ac542f514ce392.3.2.16.1731(long param_1,ulong *param_2,long param_3);
char ifio_aab21.edc1fab5bf17183c060e5a3d24eab9edd72.3.2.16.1731(long param_1,long param_2,long param_3);
void ifio_8fed0.87f6b5bd947d5612c06c76c484ce8e516ce.3.2.16.1731(long param_1,ushort param_2);
bool ifio_3d9c8.ac599f799f9147d35b0599ae2ffc14a5770.3.2.16.1731(long param_1,uint param_2,int param_3,ulong *param_4);
undefined8 ifio_a4d28.af99937ef323a6ba98ee07bd248222b4db4.3.2.16.1731(long param_1,int param_2,undefined4 *param_3);
bool ifio_94eae.bcd1c13bd2dd72458bbb4da3ca34dead16e.3.2.16.1731(long param_1,long param_2,int param_3,uint param_4,ushort *param_5,byte *param_6);
undefined8 ifio_b0749.8320a42092640eb2200e4f3ff131257a2ce.3.2.16.1731(long param_1,uint param_2,int param_3,ulong *param_4);
undefined8 FUN_00143a10(long param_1,uint param_2,uint param_3,uint param_4);
ulong FUN_00143d20(long param_1);
ulong FUN_00144290(long param_1);
int ifio_34d3e.f9a6de098bc59ce47e48780a7beb613f565.3.2.16.1731(uint param_1,long param_2);
ulong ifio_29d22.efd4f67ca98e43e8213b5177122491c4c21.3.2.16.1731(long param_1,ulong param_2,uint param_3,ulong param_4,byte param_5,int *param_6);
void ifio_e2e22.8e0b43ae9beb029323834cae07f719028fc.3.2.16.1731(long param_1);
void ifio_e2459.30d927ada9da9d78e34704ccc8a03050167.3.2.16.1731(long param_1,uint param_2);
void ifio_4c226.6332396e348218c2f7c2536ab42170234f1.3.2.16.1731(long param_1);
void ifio_6b581.a49ba03a58997a20ef1fc4c1cd2c1b269f7.3.2.16.1731(long param_1);
void ifio_25f47.5b4ae20f54665d9c8a95c8cb3466b571669.3.2.16.1731(long param_1,int param_2);
undefined8 ifio_8d901.3c205f02db06394c9810302bc068d4edbb4.3.2.16.1731(long param_1,byte *param_2);
void ifio_5854d.9a43e6cb7310d372f4095daa5b4f2c6edd1.3.2.16.1731(long param_1);
void ifio_3643f.777969b69847335485890459282f0c22a83.3.2.16.1731(long param_1);
int ifio_79716.f0be054db517598d9cafba5efcf56fa4e60.3.2.16.1731(long param_1,long param_2,int *param_3,undefined4 *param_4,undefined4 *param_5);
void ifio_26a0d.4efa140a2445980a915fdf7f457e4f58ecc.3.2.16.1731(long param_1);
undefined8 ifio_2fc91.7198d81edcd54ebd4df9ee758ba61f29383.3.2.16.1731(long *param_1);
undefined8 ifio_7a433.a7b66c96d46dc45dd3ac5affd94db35f807.3.2.16.1731(long param_1,uint param_2,uint param_3,uint param_4);
undefined8 ifio_ae10f.c4bef47c3f71bdf54b85e3d5904b59e7db7.3.2.16.1731(long param_1,uint param_2,uint param_3,undefined *param_4);
void ifio_8f9b6.9645504874a494472c0d60c437d8c0d5d16.3.2.16.1731(long param_1);
undefined8 ifio_edcff.8cf93604dfd28c2c5e192d70087460f7b5f.3.2.16.1731(long param_1);
undefined8 ifio_8f406.db9914f4ba64991d41d2470250b4d3fb4c6.3.2.16.1731(long param_1);
undefined8 ifio_9d8bf.ef7d54905d809d9c4de65d883b1c2918b31.3.2.16.1731(long param_1,byte *param_2,char param_3);
uint ifio_91449.726058b41d546bf05348495f0ab97913361.3.2.16.1731(long param_1,byte param_2);
void ifio_06202.09b0fc4f3f139a5a9a6543a059230715e31.3.2.16.1731(long param_1);
int ifio_420ca.ffd39cda33610af8ac8856d8bdcef7a2ffb.3.2.16.1731(long param_1,byte param_2,byte param_3,byte param_4);
undefined4 ifio_a5984.a1663b8771fd70627645edfe63168edfeba.3.2.16.1731(long param_1,byte param_2,byte param_3,byte *param_4);
void ifio_433b2.a03dd23c8899bf3b48cefef48687bc5e2e3.3.2.16.1731(long param_1);
void ifio_ac055.93fb2656512b9f2b078612ef6428eea7030.3.2.16.1731(long param_1);
void ifio_e6329.0f97c368d907c8951a53245ea7286c85f92.3.2.16.1731(long param_1);
void ifio_40793.649d3c29659cd041cf8b8aed2b498967e75.3.2.16.1731(long param_1);
void ifio_b2255.ddd9b6b41977d69d57253f24d93a6367b98.3.2.16.1731(void);
undefined8 ifio_d8842.f5f2e9bc5409617b505491f001dc8efe7ed.3.2.16.1731(long param_1);
undefined4 ifio_dc8f3.72cc4ab0c26d5982fea48ed5d7c9c2e253e.3.2.16.1731(long param_1);
undefined8 FUN_0014bc90(long param_1,undefined8 param_2,long param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6);
undefined8 FUN_0014c120(long param_1,uint *param_2,long param_3,ulong param_4,undefined8 param_5,undefined8 param_6);
undefined8 FUN_0014c2d0(long param_1,undefined8 param_2,long param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6);
undefined8 FUN_0014c310(long param_1,undefined8 param_2,long param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6);
undefined8 FUN_0014c360(long param_1,undefined8 param_2,long param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6);
undefined8 FUN_0014c3a0(long param_1,undefined8 param_2,long param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6);
undefined8 FUN_0014c3e0(long param_1,undefined8 param_2,long param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6);
undefined8 FUN_0014c420(long param_1,undefined8 param_2,long param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6);
undefined8 FUN_0014c460(long param_1,undefined8 param_2,long param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6);
undefined8 FUN_0014c520(long param_1,undefined8 param_2,long param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6);
undefined8 FUN_0014c580(undefined8 param_1,undefined8 param_2,long param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6);
ulong ifio_e5d29.e4d47fadcf68a8eec26d94254e82677b013.3.2.16.1731(long param_1);
undefined8 iodrive_proc_teardown(long param_1);
undefined8 ifio_956f6.f2f0ae53e51ef64ecb04d86e19fc6d78948.3.2.16.1731(long param_1,ushort *param_2,long param_3,uint *param_4);
undefined8 ifio_31418.c9a4aa1bc1ce80fe7d57c9a31912549a5a2.3.2.16.1731(long param_1,ushort *param_2,long param_3,uint *param_4);
undefined8 ifio_2f87e.a4b1a0fbc11951fabd83017038811c53467.3.2.16.1731(long param_1,ushort *param_2,long param_3,uint *param_4);
undefined8 ifio_a93fe.3ef022b46c19a883fb30e9f054afe35c5fd.3.2.16.1731(long param_1,ushort *param_2,undefined8 param_3,uint *param_4);
undefined8 ifio_61f6f.83a8b2452d523f9d6d66c1cd91cb7502942.3.2.16.1731(long param_1,ushort *param_2,long param_3);
int ifio_017ae.eb6a3f945aa6e4a861f917f345c420c0120.3.2.16.1731(long param_1,int *param_2,long param_3,uint *param_4);
void ifio_ff7df.c60a2d2be62cdcbd997bd7b1f0f63307535.3.2.16.1731(long param_1,long param_2);
undefined4 ifio_a28be.95c2d84de988f5c90b2b57e108c96a341c8.3.2.16.1731(long param_1,long param_2);
void ifio_101b0.0e45c167990d84d24a3e59c668d5831a2d4.3.2.16.1731(undefined8 *param_1);
void ifio_8de67.c987b70420a14dd1617dd23203727a41283.3.2.16.1731(long param_1);
ulong ifio_c44f3.63a5fc7dc97e40ae0e7a668c1f53309b1d1.3.2.16.1731(long param_1,long param_2);
int ifio_6fe8b.47cd24a0850269b491ad1dd3ab31ca62cee.3.2.16.1731(long *param_1);
undefined8 ifio_923e7.b7505c6ac62db2a108f2dd4a335eb9acaf3.3.2.16.1731(long *param_1);
void ifio_120fc.7bbf2582efdfd57d69c756c48e158de92fa.3.2.16.1731(long param_1);
void ifio_d6a57.6610d24e103ec1eec2461722ec6b39cb776.3.2.16.1731(long *param_1,long param_2);
void ifio_3211a.7013a3ea324a3d942785e65dbd6f1f8604c.3.2.16.1731(long param_1,char *param_2);
void ifio_af5a3.db505d57259cd6a1a1bb9f653e05f116922.3.2.16.1731(long param_1,undefined8 param_2);
undefined8 FUN_00151020(long param_1,undefined4 *param_2,ulong param_3,char param_4);
void ifio_83bc2.3305a3b63f07500e0962ef2f1c62328dcc7.3.2.16.1731(long param_1);
uint ifio_e4a95.8022ffde35e5534f16c2d452be90e1700b3.3.2.16.1731(long param_1);
void ifio_893b1.55cda86b94c0241fba192a1c9f16ce306a4.3.2.16.1731(long param_1);
void ifio_4fde5.45df87194762665eadebd5033d4ce57f9d6.3.2.16.1731(long param_1);
void ifio_0c0b9.a02329cf64b7d5be60b070a8766580c63df.3.2.16.1731(long param_1,uint param_2);
void ifio_c1ccc.424c25ab2403f5a99d8c0a32160c7a116ed.3.2.16.1731(long param_1,uint param_2);
void ifio_1a6bc.1d6276234a584e3ac9b6ec429882418fbcf.3.2.16.1731(long param_1,int *param_2);
int ifio_fb5ed.cd28b17c9d44bf73eb6f3b0276ff31c7079.3.2.16.1731(long param_1);
void iodrive_intr_fast_pipeline(undefined8 param_1,long param_2);
void ifio_5b122.e9ca5b21d35402def3a488e9490397f111b.3.2.16.1731(undefined4 *param_1);
int ifio_67751.975845ad2e8d7f490d08ea1025ccb28d20b.3.2.16.1731(long param_1);
undefined8 ifio_7d5b5.376effffe5e80284a3475e0652dced5adf6.3.2.16.1731(undefined4 *param_1,int param_2,int param_3,undefined8 param_4,long param_5,undefined8 param_6);
uint ifio_b3f70.8ad92170bce6c59ac6c5666606f2231348c.3.2.16.1731(long param_1);
void ifio_2e6b2.6b3891c06fc8e3cb337a45454968f12908d.3.2.16.1731(long *param_1,long param_2,int param_3,uint param_4);
void ifio_955ec.4b5311199b073622296dae01ab9c27594ab.3.2.16.1731(long param_1,int param_2);
void ifio_0a771.b8a7fc9c5433dd5d22d9c5962009cd52002.3.2.16.1731(long param_1,uint param_2);
undefined4 FUN_00153230(long *param_1,ushort param_2,char *param_3,ushort *param_4);
int FUN_001534f0(long *param_1,long param_2,uint param_3,long *param_4);
int FUN_00153620(long param_1,long *param_2,uint param_3,long *param_4);
int FUN_00153a10(long *param_1,undefined4 param_2,long param_3,ushort *param_4);
undefined8 FUN_00154000(long param_1,ushort param_2,ulong *param_3);
undefined8 FUN_00154380(long param_1,ushort param_2,uint param_3);
undefined8 ifio_6bad1.82758944b0e50b2d19f66f0bbb211ce8474.3.2.16.1731(long param_1,int *param_2);
undefined2 *ifio_66c93.b7e17e24626a2466e296dff8aa9151532f3.3.2.16.1731(long param_1,undefined2 param_2,uint param_3,int param_4);
void ifio_03388.5388fb596b531aea482fc2546b72e9f04bc.3.2.16.1731(long param_1);
uint ifio_7064d.207105204d8e4f955d42b3df8b7cf0ed531.3.2.16.1731(uint param_1,uint param_2,uint param_3);
void ifio_9d3a1.b97732a7f3ea403681dcacf7ea3a1257d7d.3.2.16.1731(long param_1,uint param_2,ushort param_3,ushort *param_4);
void ifio_a1e9c.7c09cc8abc53f8c84f823492045e58bb10a.3.2.16.1731(long param_1,uint param_2,ushort param_3);
void ifio_f30d3.ff6da159bdce3a2f7eea0c3387e55996271.3.2.16.1731(long param_1,ulong param_2);
void ifio_bc873.5dcef3fdfc260475814bc1226fff6d922b2.3.2.16.1731(long param_1);
undefined8 ifio_d50da.9baadf192271ff0191697656a53140f4813.3.2.16.1731(long param_1,undefined8 param_2);
void ifio_d84ea.5140a3bb9e25fd9be04fba7b27430b890c9.3.2.16.1731(long param_1,char *param_2,ushort param_3);
int FUN_00155c20(undefined4 *param_1,long *param_2,ulong param_3);
int FUN_00156290(undefined4 *param_1,long *param_2,ulong param_3);
undefined8 FUN_001563e0(long *param_1,char param_2);
int ifio_7b472.f9dab2bfb0a9c82181faeff2b8fc29770f3.3.2.16.1731(long *param_1,long param_2,ushort param_3);
bool ifio_2fde4.0c7ffab111f47c2d108e45125c66970f908.3.2.16.1731(long param_1,long param_2);
undefined8 ifio_93459.b2e1929e30308a99c2374bdf587764d5552.3.2.16.1731(long param_1);
ulong ifio_f8727.2494be6f7074c05cc6528eb620764b24e83.3.2.16.1731(long *param_1,char param_2);
undefined8 ifio_1b663.d1327094a4ae733c28924472100b8043e7e.3.2.16.1731(long *param_1);
int ifio_c6ef4.271c0a8d30dfa0dafc878703f392bf68803.3.2.16.1731(long param_1,uint param_2,int param_3);
void ifio_d111e.e7dbf2276df257b1641b8a3f2fa07911ac2.3.2.16.1731(long param_1,uint param_2,int param_3,int *param_4,int *param_5);
undefined8 ifio_4c767.3cd8343a156b76aeca42ffc3b25b2d4a65e.3.2.16.1731(long param_1,uint param_2,uint param_3);
int ifio_2f7f0.baa7be8985509c98ff4bf1d3fd64de6e6be.3.2.16.1731(long *param_1);
int ifio_40d21.ae1bc1173eccbd116a20747933b6bc1295f.3.2.16.1731(long *param_1);
bool ifio_441d0.ca1639cb10313a891accf3309de8df5889f.3.2.16.1731(long param_1,ushort param_2);
ulong ifio_cdda4.815338512f2bcf7130d06daa344d8708574.3.2.16.1731(long *param_1,ushort param_2,ulong *param_3);
void ifio_8869f.a597dda424355100c835bb219736363ac23.3.2.16.1731(long *param_1,ushort param_2);
undefined4 ifio_d013c.20841ecc755f312382b0c151c5f7bc808d1.3.2.16.1731(long *param_1,undefined8 param_2,uint param_3,uint param_4,uint param_5);
uint ifio_bfb9e.eeb57eb0363f220fff761694e4b0b0ca628.3.2.16.1731(long *param_1,uint *param_2);
ulong ifio_ff84f.f44cdd7372562797ad9838503f314db3bdb.3.2.16.1731(long *param_1,char param_2);
undefined4 ifio_6027a.04664223e7482cf83a8304182336cd52d60.3.2.16.1731(long param_1,long param_2);
undefined4 ifio_bb426.1e33244112c9a16d01bfe609c17f4dbd7dc.3.2.16.1731(long param_1,ushort param_2,uint param_3);
uint ifio_d61b7.8f77c1f9874f8ce029e721dc57326a17a66.3.2.16.1731(long param_1,ushort param_2);
uint ifio_626d1.d995a934539de4164dfe72b18f479f023a6.3.2.16.1731(long param_1,ushort param_2);
undefined4 ifio_4f4f5.f9d6f7e94c640054d403085567d28e282fc.3.2.16.1731(long param_1,ushort param_2,uint param_3);
undefined4 ifio_bc361.8941e07ffa8b95776edc5fe14cc4aa9a9d2.3.2.16.1731(long param_1,ushort param_2,uint param_3);
undefined4 ifio_f0784.52fcd803575fcaff53489f45f4678f98740.3.2.16.1731(long param_1,ushort param_2,uint param_3);
undefined4 ifio_4f3df.66a3b4b2cc7538ef05d2fa94eb14455dfe0.3.2.16.1731(long param_1,ushort param_2,uint param_3);
long ifio_f6934.15b3f1b5931e8395339647d5ce213bd387c.3.2.16.1731(long param_1,ushort param_2);
void ifio_77515.9aa5c60903c15798a9d69350fc77f716426.3.2.16.1731(long param_1,undefined8 param_2);
ulong ifio_b2f81.52df9d587f4434210e6f96dab0595ab26e9.3.2.16.1731(long *param_1,undefined8 param_2);
uint ifio_b1dcd.cf2d8507e7067b5d1fa8c7c5b80e9d58e02.3.2.16.1731(long param_1,ushort param_2,char param_3);
void ifio_c57e9.ffcc1254d587dd1ec899b56e488b7881939.3.2.16.1731(long param_1);
void ifio_e5741.c5a7823848aa831f0794f678549681a9515.3.2.16.1731(long param_1,int param_2,uint param_3,uint param_4,int param_5);
void ifio_cf235.829770e4aba9a5d608861703628650f1ddb.3.2.16.1731(long param_1,byte param_2);
void ifio_70a67.8afe740e85365e1409b9c36c4c388086478.3.2.16.1731(long param_1,byte param_2);
void ifio_4c4b8.8848eb9acafc9f38f3ac30c9d7aeb5893a1.3.2.16.1731(long param_1,byte param_2);
void ifio_8f0fa.f7bc1c64734f749401ac76e44d386906a2a.3.2.16.1731(long param_1,byte param_2);
void ifio_37cd2.109fe01d18d06e3550cd571dce1cf3646be.3.2.16.1731(long param_1,undefined *param_2,char *param_3,undefined *param_4,char *param_5,undefined *param_6,char *param_7);
void ifio_10155.995c5b534e3c795d6399747b0aca1f88137.3.2.16.1731(long param_1,int param_2,uint param_3,uint param_4,int param_5);
void ifio_02747.a32eb0f3b2d590abfd89b112691d32e1eb0.3.2.16.1731(long param_1,uint *param_2,uint *param_3,uint *param_4,uint *param_5);
void ifio_50850.515fe3cc27e9907ec64eda3c5dad53b76ca.3.2.16.1731(long param_1,byte param_2,byte param_3,int param_4);
void FUN_0015aa70(long param_1,undefined4 param_2,uint param_3);
void FUN_0015ac30(long param_1);
void ifio_35cc0.ff8337e541cd6c28d1bc546b0d04271ce64.3.2.16.1731(long param_1);
void ifio_93b4f.54b2de28b781484e108f557512ee998a68c.3.2.16.1731(long param_1);
void ifio_67ba0.334fdf247e35835c4321c81d2fd82a9a64e.3.2.16.1731(long param_1,uint param_2);
void ifio_e69a0.88020d6ed5e982f23f5a39e8d6b5088793b.3.2.16.1731(long *param_1,undefined8 param_2);
void ifio_273a3.7f0e57f8a2bf062104bd63705f6a84c1239.3.2.16.1731(long param_1,undefined4 *param_2);
void ifio_91798.f03e57d476c5a42fccd3c2fcdd96be2f349.3.2.16.1731(long param_1,char param_2);
void iodrive_dump_all_software_state(void);
void ifio_c7835.8f91c81721a431e944ea22c304a59142e48.3.2.16.1731(long param_1);
void FUN_0015d4e0(long param_1);
void ifio_1cc32.cfe63d7006e73cf6af72cf79985bc07ab76.3.2.16.1731(long param_1,char param_2,long param_3,char param_4);
void ifio_44e9b.949ac46ab9cc0444d8d9d9146cd70eec09a.3.2.16.1731(long param_1);
void iodrive_dump_all(void);
undefined8 ifio_85a3d.7a7c8eccb7fa347e03a079a47254be4ae21.3.2.16.1731(long param_1);
void ifio_072ad.78a8eb4176462dd99ecc8e1eb82ab8b04b1.3.2.16.1731(long param_1);
ulong ifio_4603e.aa04335b606706eee6648d6d94514d8429a.3.2.16.1731(long param_1,undefined4 param_2,int param_3,long param_4);
ulong ifio_f1b65.def3c293cd1a3c23d004b8c9acb060a1008.3.2.16.1731(long *param_1);
undefined8 ifio_6f066.6ee57afca8cbb3b6180d6a027a7b4bacbcc.3.2.16.1731(long *param_1,char *param_2);
void ifio_54dd8.88187029631c89bbcd9a9631a71089b5d5e.3.2.16.1731(long param_1,undefined8 param_2);
ulong ifio_eafa5.f1f3f422e93be1fa4c8f9a12c1c6c7e3725.3.2.16.1731(void);
int init_fio_iodrive(void);
void cleanup_fio_iodrive(void);
void ifio_d158d.eafd8ac70793a2834922822c381ef6a1a5f.3.2.16.1731(void);
undefined8 ifio_33887.bf0f76bb3ab19a67b86e4d345d19d153811.3.2.16.1731(long param_1,uint *param_2,undefined *param_3);
void ifio_b279e.3f9fc798e9623349d391d63f79ac0733f87.3.2.16.1731(long param_1);
void ifio_af9ad.8cf5bd672c71fd40eedc985ca7cc7e079f1.3.2.16.1731(long param_1,long param_2);
ulong ifio_fff3e.e8831c3ed098f148f5b56c0f2c9c45a7b03.3.2.16.1731(int *param_1,int *param_2);
undefined8 ifio_a8a5a.6e29b8743a1b66b07adaa1fca71d268e1f4.3.2.16.1731(long param_1);
void ifio_87254.d8c126c16ac4c64f14d5a94432e30c878b6.3.2.16.1731(long param_1);
int ifio_4ff0d.a6426dbbaeb6d84339a51ac7f25b06a747e.3.2.16.1731(long param_1,int param_2,byte *param_3,uint param_4,int param_5);
int ifio_41308.1e0cd4413cd0d44039313078fbf4c23b098.3.2.16.1731(long param_1,char param_2,byte *param_3,uint param_4,int param_5);
void ifio_39d8b.c04eb3d0c2e7c59f12bbc840b31140277d0.3.2.16.1731(long param_1);
void ifio_0b28c.6ae5c40457a35d09a09505d904a9ac588eb.3.2.16.1731(long param_1);
void ifio_62cfd.7cb572bded5342377d42bafc7dc97f853cb.3.2.16.1731(long param_1);
void ifio_19628.cd0b61e57a9a351a7982e606dde446d1d61.3.2.16.1731(long param_1);
void ifio_f1604.fa433f5aceabc9c1f54f1f26f8eb587c1af.3.2.16.1731(long param_1);
void ifio_c304d.36a67ac4c5884d5086cd0402e76ac4ad3a3.3.2.16.1731(long param_1);
void ifio_fd89a.b9c3e690bf5e757e90bb26f6b7299093984.3.2.16.1731(long param_1);
int ifio_f5608.282c8a5b7efa9169d91550e5b03fd3642d8.3.2.16.1731(long param_1,int *param_2,long param_3);
int ifio_ee604.3d31cd5914b1129bfe3b4e40dfbeae13135.3.2.16.1731(long param_1,int *param_2,long **param_3);
int ifio_9944f.96db4f153792a6338911604ef1f74876981.3.2.16.1731(long param_1,int *param_2,undefined8 param_3);
uint ifio_00c2c.64531b6396c3e80fc62f2cd253a8c4c3598.3.2.16.1731(long param_1,undefined4 *param_2,ushort *param_3);
int ifio_3690c.607bf3cccbcc3ed6d8dcad45fd3a53b036a.3.2.16.1731(long param_1,char param_2,uint param_3);
ulong ifio_8dcb3.b56de7317d7754e20735cf171e4f26e2ecd.3.2.16.1731(long param_1);
int ifio_7593e.6fb1e2220c75b6aa0c5df769f16caebc02c.3.2.16.1731(long param_1,byte *param_2);
undefined8 ifio_c8bb0.3cc8f9118207502c32a5757602b9d290129.3.2.16.1731(long param_1,char *param_2);
undefined4 ifio_5373d.58aa8f6211d7273d4a1fa9124f38c7c7440.3.2.16.1731(long param_1,long param_2);
undefined4 ifio_b23cc.d23c258cc6d83b7a3642db01cadc7cf5801.3.2.16.1731(long param_1,undefined *param_2);
undefined4 ifio_a6781.85f2947804a9836f6a992a385179cf0eb62.3.2.16.1731(long param_1,ushort param_2,long param_3);
undefined8 ifio_1f11f.6408545300d793492755637db52a6a2a9ef.3.2.16.1731(long param_1);
int ifio_ffc48.212e1f85e298b6491923a9655a7fab3c372.3.2.16.1731(long param_1,long param_2);
void ifio_5ff22.7158024b1d036cf49f37c3b0762e69889aa.3.2.16.1731(long param_1);
undefined4 ifio_99941.c3ee1f33cd766bffc9c4ef5a96c974fe00e.3.2.16.1731(long param_1);
undefined8 ifio_104a0.35e389b751ccec0eee9377120b0328bf761.3.2.16.1731(long param_1);
int ifio_9b688.e339f00ac64b6d4de2e85144e109adb7a2e.3.2.16.1731(long param_1,long param_2);
undefined8 FUN_0016a2e0(long param_1,ushort *param_2);
undefined8 FUN_0016a510(long param_1,ushort *param_2);
ulong FUN_0016a840(long param_1,ushort *param_2);
int ifio_7c645.77dda7a9ed137cd53b151963ef9e151d29a.3.2.16.1731(long param_1,ushort *param_2);
undefined8 ifio_21f66.36cbce7f3b3d65023be2388268e02b70cbd.3.2.16.1731(long param_1,int *param_2);
void ifio_776c2.59dd20e62f3f8441fba031998a522e3b3a9.3.2.16.1731(undefined2 *param_1);
undefined ifio_b7e63.1a0b62e9c76e056af18006e68e4e18815b8.3.2.16.1731(long param_1);
ulong ifio_66dfa.bee75198ac2233e36fcb9ccbd6f81153aed.3.2.16.1731(char *param_1,int param_2);
void ifio_041df.6fb7ac0e13ad5959cb3ec4e4779293d66dd.3.2.16.1731(undefined8 param_1,undefined2 *param_2);
long ifio_25e89.c5e31b52dd0f94957938ec0e2fb5eab12f9.3.2.16.1731(ushort *param_1);
char * ifio_575b3.b82767616546057fad797bb494c9cd67b23.3.2.16.1731(ushort *param_1);
long ifio_b0eb4.82c241ec1f788fa2e1ae211d2496d5c23ae.3.2.16.1731(long param_1,undefined2 *param_2);
char * ifio_79958.0d9576718db2ebce58b28b3eae5b0c77546.3.2.16.1731(ushort *param_1);
char * ifio_a629f.cd08770401f4ca1164c0673b8f892cdd000.3.2.16.1731(long param_1,char param_2,ushort *param_3);
undefined *ifio_e81a2.0e03651883515a27f5b501dd7144654ba8f.3.2.16.1731(ushort *param_1,undefined param_2);
undefined8 ifio_d107b.4864339d7c567c630d744ceee7dafb7083f.3.2.16.1731(short *param_1,long param_2);
undefined8 ifio_749f8.3fb329270e9305381e8bb73d86816bed6d7.3.2.16.1731(ushort *param_1);
undefined8 ifio_0e7d8.e2ec92155c226062e34e99f688a3803f387.3.2.16.1731(ushort *param_1,long param_2,undefined param_3,undefined param_4);
undefined8 ifio_7a770.851e857d454b6ac4a022329572a6dafc98e.3.2.16.1731(ushort *param_1,long param_2,undefined param_3,undefined param_4);
undefined8 ifio_61b00.00a43c3dc78be3240fece37fd311ff66350.3.2.16.1731(ushort *param_1,long param_2,undefined param_3,undefined2 param_4);
undefined8 ifio_681da.a64468c3082f517544b7d87aab71183e5c4.3.2.16.1731(ushort *param_1,long param_2,undefined param_3,undefined4 param_4);
undefined8 ifio_f78e8.1cf25832615a19b1ac91ad51c60fecc4c86.3.2.16.1731(ushort *param_1,long param_2,undefined param_3,undefined8 param_4,undefined8 param_5);
undefined8 ifio_246c0.9ce0c539ee1c08a13a17246bfe0ee0538d5.3.2.16.1731(ushort *param_1,long param_2,undefined param_3,undefined8 param_4);
undefined8 ifio_64c77.1c415254a9ceeee21c7d3ca06d6b543cbb6.3.2.16.1731(ushort *param_1,long param_2,undefined *param_3);
long ifio_5e9ca.af6253c910d53ba048f250f57a5ef11f4de.3.2.16.1731(ushort *param_1);
long ifio_0b3d7.7d38d5e4e15f377a2367fdfaa038475a601.3.2.16.1731(ushort *param_1);
long ifio_3af9f.e43a936b2e397ff691f13b5d88e24fbd196.3.2.16.1731(ushort *param_1);
char * ifio_014ba.d60cd7f379cf7b572cb27a3b0418dff5687.3.2.16.1731(ushort *param_1,char param_2);
undefined ifio_133cf.cf2a9cba9d18f47f2271bd22a2225ef5b20.3.2.16.1731(undefined *param_1);
bool ifio_08316.2bdfd87b87dc646d5735be8ec3b00527513.3.2.16.1731(long param_1);
undefined ifio_84f79.d02a001f12e48ec9e4d5482f975da477835.3.2.16.1731(long param_1);
undefined2 ifio_470d9.a0f8c624c64215c7cdcc85d9a067d7f6b83.3.2.16.1731(long param_1);
undefined4 ifio_0bf94.ff22a9cc3a12ee9f4476d5a1724db413d90.3.2.16.1731(long param_1);
long ifio_bd30b.b043c17f134b41daab1d6c9cc26f05af12e.3.2.16.1731(long param_1,long param_2,long param_3);
undefined ifio_43b26.4a711a842602648918720743b5981ae8f33.3.2.16.1731(long param_1,undefined8 param_2,ulong param_3);
void FUN_0016bb70(long param_1,int param_2,uint param_3,uint param_4,int param_5,uint param_6,uint param_7);
void FUN_0016bd50(long param_1,uint *param_2);
void ifio_4ab97.03cc148961b508afba4125e44b67adfa197.3.2.16.1731(long param_1,long *param_2,uint param_3);
void ifio_e013e.8b0e4e5c5a985f9b5c81f6c14732cbf0e31.3.2.16.1731(long param_1,long *param_2,ulong param_3,uint param_4);
void ifio_2b913.ed5b7bafa0d7ae934bbb9ef96ac8eaac797.3.2.16.1731(long param_1,long param_2,uint param_3);
void ifio_ffc64.686745f3348735f6eddbb2c5ff79dc2530f.3.2.16.1731(long param_1,long *param_2,ulong param_3);
void ifio_ef224.1a9e923446c5e39f397c7e7ede7acf40d3d.3.2.16.1731(long param_1);
void ifio_f0a06.dfce128155e76778e28287304f9fe1727c6.3.2.16.1731(long param_1,undefined4 param_2);
void ifio_2b4ed.2f1d78be76462658689ccbfb41c477d5c8f.3.2.16.1731(long param_1,ulong param_2);
void ifio_e1e1f.0fac545c28252842f5c7e937d2ed3be2273.3.2.16.1731(long param_1,long param_2);
void ifio_0b1b1.f06c00b0aad028e8b85fc992f31cb0d5c14.3.2.16.1731(long param_1,long param_2);
ulong ifio_5aeea.3af1248c7660ad0096a166f4f9f2af7de98.3.2.16.1731(long param_1);
void ifio_dce1e.0e6f09b10c367601920cef9fc998135d6e9.3.2.16.1731(long param_1,int param_2,byte param_3);
void ifio_8a0a6.88e926c9a56a69bd4c21185d6fe740c4ee6.3.2.16.1731(long param_1,uint param_2,undefined *param_3);
void ifio_14abe.05adb4c7de4b3f56ae5369503e3df280995.3.2.16.1731(long param_1,byte param_2,uint param_3);
undefined8 ifio_b1f23.1321cc0d79044fe300a4919414eccd57637.3.2.16.1731(long param_1,uint param_2,uint param_3,undefined2 *param_4);
uint ifio_1982c.fb1936947225a6cf25e3e377bfab61c4f94.3.2.16.1731(long param_1);
void ifio_0188f.5f4413bab23459da4e01bcf9208675e3b25.3.2.16.1731(long param_1);
uint ifio_851d2.4c5e25889f78011cee3eef4285f13b42281.3.2.16.1731(long param_1);
void ifio_66c7b.e13861a994cf5d0a9bb3f2e781e874c2b87.3.2.16.1731(long param_1);
void ifio_42c17.134a9b92e167f030851758be90617188b0c.3.2.16.1731(long param_1);
void ifio_47eb0.1505bd14988e374c74cb22d899a6f2482da.3.2.16.1731(long param_1,char param_2);
void ifio_765fb.749257b0de6517648b8c9cb8f162ff80c85.3.2.16.1731(long param_1,long param_2,char param_3);
undefined8 ifio_eea47.b741954007ce682398bbfb9b1faa69cfbd0.3.2.16.1731(long param_1,undefined8 param_2);
int ifio_9bd34.4252d923ee0c9acb5b969a9aa60b91bc73a.3.2.16.1731(long param_1,undefined8 param_2,long param_3);
undefined8 ifio_624d6.9256b17822a108e5cbfe1f3dc8213cacc11.3.2.16.1731(long param_1,int param_2);
void ifio_d7c1d.9898d2c255ad8689a7243ac5c56839b0555.3.2.16.1731(long param_1);
uint ifio_ae7c4.86bfab58895bf044145f87156ab01bd2c49.3.2.16.1731(long param_1,long *param_2,long *param_3,int *param_4,undefined8 param_5,uint param_6);
uint ifio_85d53.a12297da243d2114ed68869b0fb4e98d51f.3.2.16.1731(long param_1,long *param_2,long *param_3,int *param_4,undefined8 param_5,uint param_6);
uint ifio_d4141.c9933355eb9bec712d99bf45252f63db91f.3.2.16.1731(long param_1,long *param_2,long *param_3,int *param_4,undefined8 param_5,uint param_6);
uint ifio_38221.e638312c54e001a2c4947d0a784384f7e97.3.2.16.1731(long param_1,long *param_2,long *param_3,int *param_4,undefined8 param_5,uint param_6);
uint ifio_5db1e.34aa8c282a55183f996e27d9bb025429415.3.2.16.1731(long param_1,long *param_2,long *param_3,int *param_4,undefined8 param_5,uint param_6);
undefined8 ifio_4b8da.645540d59093fb1aa24a6b0a6899af84e6e.3.2.16.1731(long param_1,int *param_2);
undefined8 ifio_ff121.d63313028ff5eb23b160b4e7882bc3596e7.3.2.16.1731(long param_1,short *param_2);
undefined8 ifio_aeb44.e20255b5337063b6110d00bc7932580786c.3.2.16.1731(long param_1,undefined8 *param_2);
undefined4 ifio_68523.ef464db42bca3592565379a5301bba1ef57.3.2.16.1731(long param_1);
undefined8 ifio_7fbb5.43d129cc4995809977de1d697966f08eebd.3.2.16.1731(long param_1,undefined4 param_2);
undefined8 ifio_85b66.b1250aa5e05e3e1644255c3803f0ffe9c38.3.2.16.1731(long param_1,uint param_2);
undefined8 ifio_13280.81a6e12eecefd0de01633847b89109d465d.3.2.16.1731(long param_1,uint param_2);
bool ifio_72be2.4eb5fe549645dc6cf500f462e732a01815b.3.2.16.1731(long param_1,uint param_2);
undefined8 ifio_4a218.3eb1a1e5dcc429b816500a55b4eeb4b7605.3.2.16.1731(long param_1);
uint ifio_e08f5.9377eeadbeb81c814934e0c242c54082996.3.2.16.1731(long param_1,ulong param_2);
undefined8 ifio_c76c4.5e56419601c4d3712d1f998de0bab8e66de.3.2.16.1731(long param_1,uint *param_2);
void FUN_00178b10(undefined8 param_1);
uint iodrive_intr_fast(uint param_1,long param_2);
undefined8 ifio_0ea3f.4c116f2d31722a2ab31401818cb414bfa67.3.2.16.1731(long param_1);
void ifio_82074.d0a1d2556fb1c39083a02f64f5770d9dc12.3.2.16.1731(long param_1);
void ifio_c1963.ed479db96b0fa9682d835c43659a91c06c8.3.2.16.1731(long param_1);
void ifio_4972f.e40d4ff86030c320c5e751aafb8a775e0a3.3.2.16.1731(undefined8 param_1);
void iodrive_pci_detach(undefined8 param_1);
undefined8 iodrive_pci_attach(long param_1,uint param_2);
void ifio_3ceed.f01c8cc883fcfde600f380eae17a39bb4ae.3.2.16.1731(void);
ulong iodrive_pci_probe(long param_1);
void iodrive_pci_remove(undefined8 param_1);
undefined fusion_fw_gen_too_old(void);
undefined8 ifio_7992f.c642a16638e5c6d4be8c8de4ce2ec214593.3.2.16.1731(undefined8 param_1,undefined8 param_2,long *param_3);
void ifio_47ca3.059d2fab2781a798ab0aeacc5ce3ffe0230.3.2.16.1731(undefined8 param_1,long *param_2);
undefined8 kfio_ignore_pci_device(undefined8 param_1);
undefined4 ifio_642c2.2075b72a659204ebf3fcb4658715c5d49ac.3.2.16.1731(long param_1,uint param_2,long param_3);
undefined8 ifio_0ce80.27485b17c4da497e4a3910fe782806bdb61.3.2.16.1731(void);
bool FUN_0017aaf0(long param_1,uint *param_2,uint *param_3,uint *param_4);
void ifio_d556b.d1b65be35262972a39dbbda0301ccdbd3fe.3.2.16.1731(long param_1);
void ifio_ce0f4.5a4a163498fbc23c2a8fed8261697274a58.3.2.16.1731(long param_1);
void ifio_3fcc2.d2c88682bd90ddfaa03de853c5af838a2bf.3.2.16.1731(long param_1);
void ifio_64bf4.b62780cd85775dac236d634a0db4730517b.3.2.16.1731(long param_1);
uint ifio_c038e.e910aadf70f1430a66d383cbb8e9cd090f5.3.2.16.1731(long param_1,long param_2,uint *param_3,uint *param_4,uint *param_5);
void FUN_0017b300(void);
void FUN_0017b310(long param_1);
void FUN_0017b470(long param_1,long *param_2);
void FUN_0017b560(void);
void FUN_0017b570(long param_1,long *param_2);
void FUN_0017bfc0(long param_1,long *param_2);
void FUN_0017c4a0(long param_1,long *param_2);
void FUN_0017d680(long param_1,long *param_2,int param_3,uint param_4,uint param_5,ulong param_6,byte param_7,uint param_8,long param_9);
void FUN_0017e100(long param_1,long *param_2);
undefined8 FUN_0017ea40(long param_1,long *param_2,char param_3);
int FUN_0017f520(long param_1,long *param_2,char param_3);
void FUN_0017f9e0(long param_1,long *param_2,long **param_3,uint param_4);
void FUN_00180bd0(long param_1,long *param_2);
void FUN_00183140(long param_1,long *param_2);
undefined8 ifio_453ef.b7a0163dd1673b9d0d4ae825593e3932ed2.3.2.16.1731(long param_1);
void ifio_9b555.a788ccbbb4b3568018978aafd6e774a0ae2.3.2.16.1731(long param_1,int param_2);
uint ifio_0c5d4.30a121430ff50db08c2f8c2dfa9bb91bacc.3.2.16.1731(long param_1);
int ifio_9c658.cc358236e820b21ccd9934dd75af4e8c40c.3.2.16.1731(long param_1,char param_2,uint param_3,byte *param_4,int param_5);
int ifio_f74ac.f48e6f254ba97cbe8207673c5cc1997ad9e.3.2.16.1731(long param_1,char param_2,uint param_3,byte *param_4,int param_5);
void ifio_ff14e.12f9ac075580677f54993dac936dc298f2d.3.2.16.1731(long param_1,uint param_2,char param_3);
void ifio_7289a.e366c1c73df17986324ca485de546dce91d.3.2.16.1731(long param_1);
ulong ifio_3ca78.fac6280d2f250894109ac8c60405de36d9a.3.2.16.1731(long param_1);
int ifio_8f92b.71d1fb9aa55a657941417e1626a2c936f05.3.2.16.1731(long param_1,uint param_2);
byte ifio_a576b.37a3c7247f189249aba3c9649dd76ee0a1f.3.2.16.1731(long param_1);
int ifio_959a3.05db444e79dfe1a737d213ce4f77b4d1dbc.3.2.16.1731(long param_1,int param_2);
int ifio_42bdc.a0094734aac442a3daa9237bbda8878f801.3.2.16.1731(long param_1,byte param_2,ushort *param_3);
ulong ifio_7ab52.06fa2db63d7de9a49a5966f93c590fb8d6b.3.2.16.1731(long param_1);
int ifio_78fbe.d6590d3d3da3b26107c24ccc16248968922.3.2.16.1731(long param_1);
ushort ifio_c0e5d.560a41ffb790dfa3f5f09ee40131eb21d93.3.2.16.1731(long param_1);
uint ifio_42630.67eb50431e1d1f102e2d317a2f9e0cc9cca.3.2.16.1731(long param_1);
uint ifio_ed91b.b90ffd7c0442fa63e32fb25ef809442b588.3.2.16.1731(long param_1);
void ifio_63087.59a222b7dc3f1cc4f166e35d34b81b506f2.3.2.16.1731(long param_1,int *param_2,int *param_3);
void ifio_ee27b.6927fad7ebfce15c5759b68ff4587bf3c9a.3.2.16.1731(long param_1,ushort *param_2,ushort *param_3);
int ifio_5137c.c107dbece1f1470f24d2bcc87ae0ad852e9.3.2.16.1731(long param_1);
void ifio_27997.cf77425e99977cd7ba602dfa83da37b79ce.3.2.16.1731(long param_1,int *param_2);
undefined8 ifio_6baa0.365f1ee5ffd63108f90a2e2b5c947fcade4.3.2.16.1731(long param_1,undefined param_2,undefined4 param_3);
undefined8 ifio_4c863.4c5a8991c07152ec979c10c9b562b534ac7.3.2.16.1731(void);
undefined8 FUN_00184c10(long param_1,long *param_2);
void FUN_00184d80(long param_1,long param_2);
undefined8 FUN_00184fe0(long param_1,uint param_2,long param_3);
long FUN_001851f0(long param_1,uint param_2,long param_3);
int FUN_001854c0(long *param_1);
long ifio_ebf01.74df8c2ab61af162ddb8d4e90e3a2d154f5.3.2.16.1731(long param_1,int param_2);
long ifio_e8377.760ac51b9e3309544404f08f748836b7972.3.2.16.1731(long param_1,int param_2);
long ifio_f582b.96e4eb32609ba52dc5c3e454be48ba0f04b.3.2.16.1731(long param_1,int param_2);
long ifio_aeaa7.a5d953a885ad88a6120d2b640db1c081fd3.3.2.16.1731(long param_1,uint param_2,int param_3);
undefined8 ifio_4d656.1c77d7b7fe37d3551b6ecdd61772db0fdd6.3.2.16.1731(long param_1,int param_2,long *param_3);
bool ifio_378b4.fda24a2fc2deed3a5b4f0517d378b3e79b0.3.2.16.1731(long param_1);
void ifio_54197.1efb41011f422a6984d340f7be11ff78d5e.3.2.16.1731(long param_1,long param_2);
void ifio_673d5.9c0df5afc3d8f0838df2366c3c554ec5a47.3.2.16.1731(long param_1,long param_2);
int ifio_2ca9f.5d0d73922432bd8f14a2355a618bed60dcf.3.2.16.1731(long *param_1);
void ifio_fb6fc.73d477631acdb77196b24d515876e94d1b8.3.2.16.1731(long param_1);
undefined8 ifio_ddc8b.73c01bfc5a34504c7b9f3cb43c0d7ba95ab.3.2.16.1731(long param_1);
void ifio_f55b4.6146072a17627fb5e158cc0249e6bb5033e.3.2.16.1731(long param_1);
void ifio_29370.c97d4a91b5584da3a245f425e0ccf931f84.3.2.16.1731(long param_1);
void ifio_54458.749271af7edc5e2b95e87eead70e38310d2.3.2.16.1731(long param_1);
void ifio_a89e6.55808fe54a59003264a7b393ca29f3b8233.3.2.16.1731(long param_1);
void ifio_05887.c7992508ffab82c808c8d31a12e901d46ca.3.2.16.1731(long param_1,char param_2,undefined8 *param_3);
void ifio_60cf2.95a785ecaf242cc76bf36a378de6f8eb530.3.2.16.1731(long param_1);
bool ifio_1e5a3.5e62afc90b8d1b25ae145e583e480994677.3.2.16.1731(long param_1,long **param_2,char param_3);
int FUN_0018b4f0(long param_1);
ulong ifio_32dfc.7345751cc80fb338fd5a2621519887db69d.3.2.16.1731(long param_1,byte param_2);
int ifio_9b38a.a6276632039526f5130faf85f36cf84e528.3.2.16.1731(long *param_1);
void ifio_fbeca.b91e0233dee7fc9112bb37f58c4e526bcd8.3.2.16.1731(long *param_1);
ulong FUN_0018cbf0(long param_1);
long * FUN_0018d660(long param_1,uint param_2,long **param_3);
long * FUN_0018e1c0(long param_1,uint param_2,long **param_3);
uint ifio_f0cee.039ff5a1c1f314a171e2a8a425361a5875d.3.2.16.1731(long param_1);
undefined8 ifio_5fb65.a9c484151da25a9eb60ef9a6e7309d1a95f.3.2.16.1731(long param_1,long param_2,undefined8 param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6);
void ifio_efb1a.38dede6842de96c9dd1d1b39f3b0447e87c.3.2.16.1731(undefined8 param_1,long param_2,short param_3);
void ifio_d7815.4fa65444c36363139f5a17962182025b282.3.2.16.1731(long param_1,undefined8 param_2,undefined *param_3);
void ifio_1b37e.6a012abcfe8248a5437b7b5e15dc8cc858d.3.2.16.1731(long param_1);
undefined ifio_3aef2.ae8bfaba6522ad3fa3843e71c69cffdd561.3.2.16.1731(long param_1);
byte ifio_bd3ab.cfc7cf497739db91bce65806d316e772cd9.3.2.16.1731(long param_1);
void ifio_7b76d.7e19ab0e27200cba109b8e04e1aedf20142.3.2.16.1731(long param_1);
undefined8 FUN_0018f1d0(long *param_1);
void ifio_e5aa8.a955d1f60a38d13e0270b53acdcb907e591.3.2.16.1731(undefined8 param_1);
undefined8 ifio_dbc8c.172c0f7020398fb8cd41123678fb31cefd7.3.2.16.1731(long param_1,byte *param_2,undefined *param_3,undefined *param_4,undefined4 *param_5);
undefined8 ifio_4baaa.b32668a32e71a952e07cf936571d466f624.3.2.16.1731(long param_1,byte *param_2,undefined *param_3,undefined *param_4,undefined4 *param_5);
undefined8 ifio_80e9b.5589240be06f367ced370d88b239d45ca46.3.2.16.1731(long param_1,byte *param_2,undefined *param_3,undefined *param_4,undefined4 *param_5);
uint ifio_88038.c6860774b96c6ee6e71f43f611d20f97b6f.3.2.16.1731(long param_1);
ulong ifio_11783.1c78ffdd4620b3f7154e6f2512c959d8cfa.3.2.16.1731(long param_1);
uint ifio_c27f5.7ceac909f917adbc22db13526634f5acffb.3.2.16.1731(long param_1);
ulong ifio_d73a1.9b6c610480b8b4d57510e1b203c86ca2b86.3.2.16.1731(long param_1);
ulong ifio_2cacb.3899dd9d84d49e5cc318f499ef7030c761c.3.2.16.1731(long param_1);
ulong ifio_69771.cf4d96bfb7d8457de6f171e0312642e89cb.3.2.16.1731(long param_1);
undefined4 ifio_487ce.fd7865d91b038a8194c2c6ebb3e07c68616.3.2.16.1731(long param_1);
ulong ifio_633c0.c002494f993d8691575ce14c81ec6e49e64.3.2.16.1731(long param_1);
undefined8 ifio_fd052.cfe1d30c2422b1a61cc4ce0414ef1ca5860.3.2.16.1731(long param_1);
void ifio_ee4cd.41b45a7ec2affa4db222bc16de250a68e15.3.2.16.1731(long param_1);
void ifio_ccb74.52bff709bf06b836644ced1e24fa9bfc469.3.2.16.1731(long param_1);
int FUN_00192500(int param_1,int param_2,uint param_3);
int FUN_001928b0(uint param_1);
uint FUN_00192ad0(uint param_1,uint param_2,int param_3);
uint FUN_00193010(uint param_1,uint param_2,uint param_3);
void ifio_4b35b.f5f6ee16148fb823ef28929c5133ae577c5.3.2.16.1731(uint param_1);
bool ifio_a3513.3404cd1aff8de58573848bc4618415e0ea0.3.2.16.1731(int param_1);
bool ifio_a9e8c.5a484c048430d3b6bdcfae58f742199a080.3.2.16.1731(uint param_1);
bool ifio_d8c49.61726d058aaf33936e6762baec87e983f99.3.2.16.1731(long param_1);
bool ifio_dc22e.29fcadd6f133f4037ce2d2991c74eab1561.3.2.16.1731(ulong param_1);
int ifio_828f6.42bba0593cc2f1398a9f14c3dda4685ce7a.3.2.16.1731(uint param_1);
long ifio_5cb09.6539747d3a3145af5e61bc11564b63e2d3a.3.2.16.1731(uint param_1);
int ifio_dd6a4.075ad47f7190d7417a06f2e0a5a7400418c.3.2.16.1731(ulong param_1);
long ifio_05af9.8799d71b9b50411dde875e9562d60ce2ffb.3.2.16.1731(ulong param_1);
ulong ifio_8d1f7.2a5c87998108fad4cbedfe6bcd009532860.3.2.16.1731(ulong param_1);
uint ifio_b5697.bcc356be8c625ac36b0ca3b80367e53ee89.3.2.16.1731(uint param_1);
ulong ifio_b1e60.e1a69b2b80b3f2a3d72a15c7a4507ec34e1.3.2.16.1731(uint param_1);
ulong ifio_2b069.a65d3a293951f6f2fc70e8eae84820d7f36.3.2.16.1731(uint param_1);
ulong ifio_bbdf9.9e26c3548572c36786691f8808aa36bc22e.3.2.16.1731(uint param_1);
uint ifio_074f3.35ac0bab8ba7eb5735ed40a86df2279a9ee.3.2.16.1731(uint param_1);
uint ifio_99397.c831f6ec92c4fe5efa8d7fcb4293d87fae0.3.2.16.1731(uint param_1,uint param_2);
uint ifio_94aeb.680153e01cb5d0c5f9c30beb397ee75c542.3.2.16.1731(uint param_1,uint param_2);
uint ifio_e9a83.997906115475dad5f32448fab68bbe16915.3.2.16.1731(uint param_1,uint param_2);
uint ifio_0d3ef.3bae51cba507d73d82e9ad39592eaadf7bb.3.2.16.1731(uint param_1,uint param_2);
uint ifio_dde2f.20d83d645786b5b6e8f5df67a9317528473.3.2.16.1731(uint param_1,uint param_2);
uint ifio_15ef0.b229c540e7be92e480f50d5b51efc6539f5.3.2.16.1731(uint param_1);
bool ifio_43fa5.3ce001062aa9954f16048d981ff8ff8b5dc.3.2.16.1731(uint param_1,uint param_2);
uint ifio_61cdf.b4fc3b3dadd3dfc51437a70a1d9dc481d35.3.2.16.1731(uint param_1,uint param_2);
bool ifio_27839.3d1fe5a44bbb52996aed8da90b71be677f1.3.2.16.1731(uint param_1,uint param_2);
bool ifio_012bd.ccdedebf9fa512561d93d6e7027acf1b9ae.3.2.16.1731(uint param_1,uint param_2);
uint ifio_68b0b.7e5e2f1882278b2301a87dd9837dda33595.3.2.16.1731(uint param_1,uint param_2);
bool ifio_2bf0e.11d87251b185b27e75aa95f00a3c6696e68.3.2.16.1731(uint param_1,uint param_2);
ulong ifio_050a7.3f37f659436b57c1ea0914327c7918a1f8a.3.2.16.1731(ulong param_1);
ulong ifio_e7a26.188910347bbfde3b8b79b65ffaf1d782340.3.2.16.1731(ulong param_1);
ulong ifio_f9311.8b9071572d84472ab5eec5f877f2a4be2f7.3.2.16.1731(ulong param_1,undefined8 param_2,undefined8 param_3,uint param_4);
ulong ifio_d50a6.77e179b73c616cadce2f97369d8672c3511.3.2.16.1731(ulong param_1);
uint ifio_9b3af.f896031fb7225e364c1900d2df5b6e4fc94.3.2.16.1731(ulong param_1);
ulong ifio_02009.d31c57490457fbc6a71498fd2873e42a9c0.3.2.16.1731(ulong param_1);
ulong ifio_0f72c.3af0410fd13554e4945af08bcffc84ed1c0.3.2.16.1731(ulong param_1,ulong param_2);
ulong ifio_c9cb9.3eb65c78d03a26568830d9d7818b0baae56.3.2.16.1731(ulong param_1,ulong param_2);
ulong ifio_38674.eed5f5ea6c756db69505ab0aba13602e01f.3.2.16.1731(ulong param_1,ulong param_2);
ulong ifio_eb19f.e29c5a20ff9edcd0d35327496f8c70aa8b3.3.2.16.1731(ulong param_1,ulong param_2);
ulong ifio_b3cb0.71ec07ca75e3fa54e9aa5b1a65e55e0b4f3.3.2.16.1731(ulong param_1,ulong param_2);
ulong ifio_e8e9c.5ab51793b34871832a79b3369144bea2030.3.2.16.1731(ulong param_1);
bool ifio_c97e0.8bbb56abfb93ccfd424ff2d6e7bc5289ad5.3.2.16.1731(ulong param_1,ulong param_2);
ulong ifio_1ed92.4eaccf2dc935032005adfb4e151c1b8d112.3.2.16.1731(ulong param_1,ulong param_2);
ulong ifio_86059.7ec1a6b60ae378612f5109dd027e56d986a.3.2.16.1731(ulong param_1,ulong param_2);
bool ifio_7a344.f824e5613d41533946560dadf98a6dcafda.3.2.16.1731(ulong param_1,ulong param_2);
ulong ifio_81d99.27aa3b93746b4e560036dd81f53fa1eef80.3.2.16.1731(ulong param_1,ulong param_2);
ulong ifio_ac7a9.f95ea488285851fd3b5356cdd60f83300fb.3.2.16.1731(ulong param_1,ulong param_2);
uint ifio_5bc79.61099c63850ba600f6bd05f2ff0390c7389.3.2.16.1731(uint param_1);
uint ifio_bd429.1932cd76dbae0133fa26f720acbff461856.3.2.16.1731(uint param_1);
void ifio_a0b59.2ff0ecbdf644b6e81af4ebd73c59e24a83a.3.2.16.1731(long param_1,uint param_2,undefined2 *param_3,undefined2 *param_4);
void ifio_23a38.9479a9ff00d4ac4b726618c80566e384587.3.2.16.1731(long param_1,int param_2,undefined2 param_3,char *param_4);
undefined8 ifio_8cc70.70818125751e76372d6e36f00328a71c937.3.2.16.1731(long param_1);
void ifio_cb295.aec8a6fe502a9f330010f9e3b87da6bdc14.3.2.16.1731(long param_1);
void ifio_d0f6a.7c1e6b477f4693e36cd5fa004855e28951c.3.2.16.1731(long param_1,uint param_2,char param_3);
void FUN_0019f180(long param_1);
void ifio_b7ce6.5cf8b389c23c385ae866990c23e2efc21da.3.2.16.1731(long param_1);
void ifio_2b18c.f4c2c36ef384acd1c5ca90c03854de647c7.3.2.16.1731(long param_1,undefined4 param_2);
void ifio_d3da7.1acc84b01f2dc1df90375fafe9510c58f97.3.2.16.1731(long param_1,int *param_2,int *param_3);
void ifio_e3415.a1a29e966be8b42278d088228aac1638c20.3.2.16.1731(long param_1,undefined4 *param_2,int *param_3);
void ifio_3c814.6dcfd51fdf471a166a2caaf6dc1b2c26592.3.2.16.1731(long param_1,int *param_2,int *param_3);
void ifio_bfc14.0fea43f1ecafe47578dba03b209d5ac7ea3.3.2.16.1731(long param_1,undefined4 *param_2,int *param_3);
uint ifio_4855e.e100d8ab4bb1d646b96d86fea7cbdc3e25a.3.2.16.1731(long param_1,long param_2,uint *param_3,undefined4 *param_4,undefined4 *param_5);
void ifio_0450b.fd412908acb5ce275cd45f6a3b5d4fa388e.3.2.16.1731(long param_1,int param_2);
void ifio_54cba.bb2592995d6b0c4aa5b19b4820596d35dc7.3.2.16.1731(long param_1);
undefined8 ifio_16556.a7bb3c534a577a9d90a60447ec060cc0f1b.3.2.16.1731(long param_1,long param_2);
undefined8 ifio_a3248.0d064e506a229f1fd7492ad1553684c2036.3.2.16.1731(long param_1,long param_2,undefined4 param_3);
undefined8 ifio_c9a44.bc7a164d2679928cbe2ce0fd7e4bc831e6f.3.2.16.1731(long param_1,long param_2,undefined4 *param_3);
void ifio_44594.9ed0181832b3fc04570ac5cbe66dad8222a.3.2.16.1731(long param_1);
long ifio_513a1.9b5477862a92443fe7322bf2518eaf47e74.3.2.16.1731(long param_1,long param_2);
long ifio_2b711.78b8f9707e5b90ea30073ffd19972fc4665.3.2.16.1731(long param_1,long param_2);
int FUN_0019fb50(long *param_1,ushort param_2,char *param_3,uint param_4,uint param_5);
int ifio_1a355.5fcd9943bfac9b636202236f4aa3769d962.3.2.16.1731(long param_1);
void ifio_5b870.5ab38a73142cc0ce79f6b1c7935b46a5421.3.2.16.1731(long param_1);
uint ifio_09f78.2845d08c8a78fe2e34de1848715d21eea17.3.2.16.1731(long *param_1,ushort param_2,ushort *param_3,ulong *param_4);
int ifio_65ca0.df48ff4a4329f80e25eea71c441b771d2d8.3.2.16.1731(long *param_1,uint param_2,undefined2 param_3);
void ifio_9e032.a3e47bc9c49bcb51fb4486253046ca10584.3.2.16.1731(long *param_1);
int ifio_8a30f.3f931e2037196fc46b5b711d5ae377442c8.3.2.16.1731(long *param_1,undefined8 *param_2);
undefined8 FUN_001a1a30(long param_1,long param_2);
undefined8 FUN_001a1d30(long param_1,long param_2);
undefined4 FUN_001a1e80(long param_1,long param_2);
undefined8 FUN_001a2140(long param_1,long param_2);
int FUN_001a2910(long param_1,long param_2,char *param_3);
int FUN_001a2f00(long param_1,long param_2,char *param_3);
undefined8 FUN_001a3290(long param_1);
byte FUN_001a3580(long param_1,char *param_2,undefined param_3,undefined4 param_4,byte param_5,undefined8 param_6);
byte FUN_001a3960(long param_1,byte *param_2,undefined param_3,undefined4 param_4,byte param_5,undefined8 param_6);
int ifio_c3104.c4ac92fb181e1ebf359a9877de3fc5d0871.3.2.16.1731(byte *param_1,undefined *param_2,int param_3);
void ifio_e1f5a.7ffb103529a90f00dc1037c46f34dce84c8.3.2.16.1731(undefined *param_1,undefined *param_2);
bool ifio_cc9c0.d6549f7b4a6dc17ea3e0c86873374400948.3.2.16.1731(long param_1,char *param_2);
void ifio_3ae4f.decb3256c9082e5ddfe401cca9b0c409d11.3.2.16.1731(long param_1);
void ifio_5e3b1.9eadd7d2a1e2b23b177d21e023c8ed93881.3.2.16.1731(long param_1);
void ifio_90202.8054f4bf7c8b28d53704c4da637e7baa4aa.3.2.16.1731(long param_1);
ulong ifio_f8753.08c67f376129f3b8920b68f816490725f83.3.2.16.1731(long param_1);
undefined8 ifio_5eec3.6e6c122de84d725c7005c65a980a1327841.3.2.16.1731(byte *param_1,byte *param_2);
undefined * ifio_6ad16.3b5560986a2f5f9f11d9a3589e7767a661d.3.2.16.1731(byte param_1);
char * ifio_f3635.df424fe41cb9d839d49d94952dfcfc4e8f0.3.2.16.1731(char param_1,uint param_2);
char * ifio_8bc16.28d912734f4d8eee72a8faef7ced27c6d15.3.2.16.1731(char param_1,uint param_2);
undefined8 ifio_f5fc5.e54cd4f1d09dd827f43856d2d70f0501097.3.2.16.1731(char param_1);
undefined * ifio_ba022.4d1b8e9d7f2292295cad22a0d3f91f9e9d4.3.2.16.1731(char param_1);
undefined * ifio_d2eb1.6e289c0b2458aa8bce990e40bade1c7ab41.3.2.16.1731(byte param_1);
undefined * ifio_ff9bd.e196dd66149573878399a0ec48e38f5b7a9.3.2.16.1731(byte param_1);
char * ifio_47fc4.1d53194a36733364c473db23645c54b5452.3.2.16.1731(char param_1,byte param_2);
ulong ifio_fc783.c96983ea2f642a35ee348d9645f6f7d76bf.3.2.16.1731(long param_1,byte *param_2,undefined param_3,int param_4,uint param_5,long param_6);
ulong ifio_2c776.ddfeaf6e033cd45a592e020425203f0e51a.3.2.16.1731(long param_1,char *param_2,undefined param_3,int param_4,uint param_5,long param_6);
int ifio_5f49b.4c48186d0b1e55ffc682a8f4e43c9cef15a.3.2.16.1731(long param_1,byte *param_2,long param_3);
int ifio_b7f92.212dff004305e064fa3a2278ab85e383cbd.3.2.16.1731(long param_1,byte *param_2,long param_3);
undefined8 ifio_35463.a3e051a56020a6395b3ee1e32f3b9545ce0.3.2.16.1731(long param_1,byte *param_2,undefined4 *param_3);
int ifio_bb121.4b4ef576e59d4cef71530b63a00357715f4.3.2.16.1731(long param_1,char *param_2,int param_3);
undefined8 ifio_4ebb5.83460277e5e4a65e0898c9b4a197b2a767c.3.2.16.1731(long param_1);
ulong ifio_7ad61.94a1beb32a799fa2e5b9dafaf734dcb040a.3.2.16.1731(long param_1,byte *param_2,undefined2 *param_3,undefined2 *param_4,undefined2 *param_5);
ulong ifio_4eddf.b002449d6c7937231d5eac064d0c30078c7.3.2.16.1731(long param_1);
void ifio_b466e.1aa200647c9c03aa38529a4c86467b21b51.3.2.16.1731(long param_1);
long FUN_001a4ed0(long param_1,uint param_2,ushort param_3);
ulong ifio_8304e.bbe9d1b16a00cdfcaa66ac85ebf96e4e031.3.2.16.1731(long param_1,undefined8 param_2,uint param_3,undefined8 param_4);
ulong ifio_4c4dd.0f847a6e303b83ac48266a378ef70d078e4.3.2.16.1731(long param_1,uint param_2);
undefined8 ifio_6d9f0.7605b22174fade54d78e697cd13d99ebd8a.3.2.16.1731(long param_1);
void ifio_cb0f5.a676a1b789a9cecfdc52b6149db418f220b.3.2.16.1731(long param_1);
void ifio_614ea.d21613cd05ca449b637717e8dc165cb3a03.3.2.16.1731(long param_1,uint param_2,ushort param_3,long param_4,long param_5);
undefined8 ifio_93505.2697a1bb031fc8e45062e981e5470dca05d.3.2.16.1731(long param_1,long param_2,long param_3);
undefined8 ifio_b2b93.f10b15f80b86098913f2f6fcbb885ff32a6.3.2.16.1731(long param_1,long param_2,long param_3);
void ifio_ea274.77e924fbf00553a77ca4054e8cdae664408.3.2.16.1731(long param_1,long param_2,long param_3);
void ifio_752b7.77158378fea72ff623c862fae57a932378b.3.2.16.1731(long param_1,uint param_2);
void ifio_76a81.350c223bbc78625ef0d51108da395a17fc4.3.2.16.1731(long param_1,uint param_2);
void ifio_29506.e224c67a6ca4d60fc1ae96a026b93d5e414.3.2.16.1731(long param_1,ushort param_2,long param_3);
void ifio_cf11d.03093419077800d50781e955256a75c88ee.3.2.16.1731(long param_1,uint param_2,long param_3);
void ifio_13616.b297da20a828929ad54e82328325aaf3422.3.2.16.1731(long param_1,uint param_2,long param_3,long param_4);
void ifio_73135.bf03faf90f5b8e2150ae7f284314d421735.3.2.16.1731(long param_1,long param_2,undefined8 param_3);
undefined8 ifio_3079a.50eb955498ed4dc1934953b37bb6ecdf239.3.2.16.1731(long param_1,long param_2,int param_3,int param_4,char param_5);
undefined4 ifio_3dc95.628757b331da783956a266bec84c17179e8.3.2.16.1731(long param_1,ushort param_2);
void FUN_001a67f0(long param_1,uint param_2,uint param_3,uint param_4);
void FUN_001a68e0(long param_1,uint param_2,undefined8 param_3,uint param_4);
void FUN_001a69c0(long param_1,uint param_2,uint param_3,uint param_4);
uint FUN_001a6ad0(long param_1,uint param_2,uint param_3,uint param_4);
void FUN_001a6c70(long param_1,uint param_2,uint param_3,uint param_4);
void FUN_001a6c80(long param_1,uint param_2,uint param_3,uint param_4);
uint ifio_ae5e9.f5264c616e834f1299ddb9bf3910bb5628d.3.2.16.1731(byte **param_1);
int ifio_e5b02.1f6496eb878d84589d22e0f0bcc5677d44c.3.2.16.1731(long param_1,undefined *param_2,uint param_3,undefined4 param_4,uint param_5);
void ifio_f7d22.dad3a7b095df54208d1ff43446ac9ef4b55.3.2.16.1731(undefined8 param_1,char *param_2,int param_3);
void ifio_a59de.ac88eb7ab97c14e8f78b1ef9f2f17f85561.3.2.16.1731(long param_1);
undefined2 ifio_ef259.c99c834570ca7325ef0e205ba69aa00a8a5.3.2.16.1731(long param_1,ushort param_2);
undefined8 ifio_33ddf.4104c62735b8baa90734448e65262e614a9.3.2.16.1731(long param_1,ushort param_2,undefined2 param_3,int param_4);
void ifio_f4ed1.5214e72feb9ef3f323ab3fbf208437251a2.3.2.16.1731(long param_1,ushort param_2);
void ifio_f0c82.d37f5123e85349832b8c419f1252746ffcc.3.2.16.1731(long param_1,ushort param_2,uint param_3,uint param_4,uint param_5);
void ifio_5dfef.f934b599e4b243a308e9eb8f69099f6fd11.3.2.16.1731(long param_1,ushort param_2,uint param_3,uint param_4,uint param_5);
void ifio_9354f.47a2822cb6c5bd74426456cf81f2e891273.3.2.16.1731(long param_1);
void ifio_358e3.9e23cab10e7c8cbff882d4ecfe6467df210.3.2.16.1731(long param_1);
void ifio_de901.f77a75ad616c46aee9fc2cca7ffc5695a46.3.2.16.1731(long param_1);
void ifio_86201.f4c92b3786b79d232d055847aeb7261f376.3.2.16.1731(long param_1,undefined8 param_2,ulong **param_3);
ulong * ifio_4106c.a1f202a733e7504e31e1c1cd1cc21e5eac0.3.2.16.1731(long param_1);
ulong * ifio_bac89.41814685988d928e3ac17b3f8a4353ac3cc.3.2.16.1731(long param_1,byte param_2);
void ifio_c71c2.b18b0d75b1f3a1a5b6aa9524c4252194631.3.2.16.1731(long param_1,int param_2);
undefined8 ifio_b8830.be8da4a69f97d8a9f4767c410a300b07a7c.3.2.16.1731(undefined8 param_1,undefined **param_2,ushort *param_3,int param_4);
void ifio_ed1d4.d1a36092b667f03f3f629e30a797548cb30.3.2.16.1731(void);
void ifio_b96b6.68eb65c7209752c93e33584f29a178164e2.3.2.16.1731(undefined8 param_1,long param_2);
ulong ifio_67370.d838e7d8c1be32bef9465c301537b583dfc.3.2.16.1731(long param_1,undefined8 param_2,long param_3,uint param_4,undefined8 *param_5,int param_6);
void ifio_f02ff.b9420b452eac46aa80a79b83170c0a5704e.3.2.16.1731(undefined8 *param_1);
undefined8 FUN_001a8da0(long *param_1,int param_2,int param_3,uint param_4);
undefined8 FUN_001a9210(long *param_1,int param_2,int param_3,uint param_4);
void FUN_001a9660(long param_1,long param_2,long param_3,long *param_4);
ulong ifio_0aebb.9e6270f543405e2ba77353b055107dc2694.3.2.16.1731(int *param_1,int param_2);
ulong ifio_fbe1c.d3c3aab74ed9aedeee7937bfe00f5d5dcac.3.2.16.1731(int *param_1,ulong param_2,ulong param_3,undefined **param_4,undefined8 param_5,char param_6,char param_7);
ulong ifio_0d0f2.7082a39e0485cc57e054663237415c007b2.3.2.16.1731(int *param_1,ulong param_2,ulong param_3,undefined **param_4,undefined8 param_5,uint param_6);
int ifio_b3118.a2f16c86ebd144440f816f9dbc87a2ab2a9.3.2.16.1731(long param_1,long param_2,ulong param_3);
int ifio_8bfaf.098ed49bc53565b025b6ee004155f86ca33.3.2.16.1731(long param_1,long param_2,ulong param_3);
void ifio_0cec9.c0f9c301d9ace75d2620649bf9b0a8fb76f.3.2.16.1731(long param_1,long param_2);
undefined8 ifio_5306e.9d9e70774f81ecd8114396ba3ec9ef4831b.3.2.16.1731(void);
ulong ifio_5bf34.170bef521fd9809a03e20175b589b4d6eb1.3.2.16.1731(uint *param_1,ulong param_2,ulong param_3,ulong param_4,long *param_5,undefined8 param_6);
uint ifio_fbd63.25e3b827471e235a784f52e7bf035f46a2f.3.2.16.1731(long param_1,long param_2,ulong param_3);
uint ifio_c89f1.f7618168df3ac6e352f409d5941b3719e42.3.2.16.1731(long param_1,long param_2,ulong param_3);
void ifio_57cfe.815193eb0a77cfa2f7887e761be9ebb97e1.3.2.16.1731(uint *param_1,undefined *param_2,ulong param_3,byte param_4,undefined param_5);
void ifio_e9bdc.27d5e6b1149c4e4befbda2a43bba2a756be.3.2.16.1731(uint *param_1,long param_2,int param_3,long param_4,int param_5,uint param_6);
long ifio_f51ab.f26e86b85cf9f0013bccfb8633088192215.3.2.16.1731(uint *param_1,long param_2,uint param_3,int param_4);
void ifio_c9d81.4151bd9cbeff50df799d9e7e7ec6a1c4c7b.3.2.16.1731(uint *param_1,long param_2,uint param_3,int param_4);
void ifio_3b3e2.edff2aee9eab65625cacee2e5201dce89f0.3.2.16.1731(void);
long ifio_12d82.b86daa02929f92f4805762f6a57953e19e8.3.2.16.1731(ulong param_1,ulong param_2,int param_3,uint param_4,uint param_5,undefined8 *param_6,ulong *param_7,ulong *param_8);
undefined8 ifio_78545.9c2721d857667f13ed8194be56b02aeca07.3.2.16.1731(void);
undefined8 ifio_adf6d.01278d16fcc1136d19a9577bcb31d8767fa.3.2.16.1731(void);
undefined8 FUN_001ac300(undefined8 param_1,undefined8 param_2,undefined8 *param_3);
ulong ifio_fba2d.8dd19bdb56517f97d9d57c5fc2ef1e8aefc.3.2.16.1731(long *param_1,undefined8 param_2,undefined8 param_3,ulong param_4);
int ifio_0a04b.3d3b95dc5cb9942224d124f85548e541c7f.3.2.16.1731(long param_1);
int ifio_e1bf7.8fda83635a27f32a53b1a6e37ad083013f0.3.2.16.1731(long param_1);
int ifio_79dc1.dd68aa3ff9455a85d22a2bb06aac97ab1a6.3.2.16.1731(long param_1);
int ifio_96ee9.7bc384875b0619f61af933bdf62a596033a.3.2.16.1731(long param_1,int param_2);
void ifio_3ad0d.b662eaca8e96089681c57d97ae66287cea9.3.2.16.1731(long *param_1);
int ifio_50f6f.2b9a95682471f628be0729aa8d27ac234af.3.2.16.1731(long param_1);
void ifio_7d929.3c5f336e8f2c0a3ec41105aa92cac8c4f2e.3.2.16.1731(long param_1,undefined8 param_2,undefined8 param_3);
int ifio_7b09e.d2e693053b9694c0de93780eb72d3efa915.3.2.16.1731(long param_1,ulong param_2,long param_3,ulong param_4,long *param_5,undefined8 param_6);
int ifio_24f29.641df62d66f7c76d6af5f3f3fd51596bb49.3.2.16.1731(long param_1,ulong param_2,ulong param_3,ulong param_4,long param_5,long *param_6,undefined8 param_7);
int ifio_32fcc.f9c92add4e41b5efd63426bde9cad7a13be.3.2.16.1731(long param_1,ulong param_2,long param_3,undefined **param_4,undefined8 param_5,uint param_6);
undefined8 ifio_2f66f.fc988f9718bfdb7f61292673e2d1674f0ec.3.2.16.1731(void);
void ifio_71721.ec210ae71737818f9b236e93e5c81b62ccd.3.2.16.1731(ulong param_1,ulong param_2,int param_3,uint param_4,uint param_5,undefined8 *param_6,ulong *param_7,ulong *param_8);
long ifio_f4ae4.22cacaa4c21b8def1f07e729eef7d6ea18b.3.2.16.1731(long param_1);
long ifio_02336.e17ebd917e2f4abba27130e722f8fe1a88d.3.2.16.1731(long param_1);
long ifio_70a94.bce9e8c06ae8fe09298a44c79ebfefa287f.3.2.16.1731(long param_1);
ulong ifio_9bab9.1c37aebac37e8ca2ec56897116b26c920f6.3.2.16.1731(long param_1);
undefined8 ifio_b80cb.4ca489cc13c519f4376e31735e0bb42950d.3.2.16.1731(long param_1,ulong *param_2,ulong param_3,long param_4);
undefined8 ifio_aca21.1a15e822bee0b1cd4689d97e0295f621ab7.3.2.16.1731(void);
undefined8 ifio_ec2a3.0763fec9e8a733af741fb626dc9a5193809.3.2.16.1731(long param_1,long param_2);
undefined8 ifio_83e9c.3aa3c4dcfa9754d6681f22cfcb2f02e75d8.3.2.16.1731(void);
uint ifio_96969.6d12b21cc70e39019cb3af460d22b6494e7.3.2.16.1731(long param_1);
void ifio_c0ab4.9dbcb0886497e878e6debada36bb4d54336.3.2.16.1731(long param_1,long param_2,char *param_3);
void ifio_793ae.fb68f9bce6e009afea43f87f965e3a9ed6e.3.2.16.1731(long param_1,long param_2,char *param_3,int param_4);
void ifio_825ec.7b4db0ec88e482f7b51c073518a069a8c42.3.2.16.1731(long param_1,int param_2);
void ifio_ccff0.b3da664c2847779dc7576d9ec4389c61155.3.2.16.1731(int *param_1,int **param_2);
void ifio_9b215.97b020fa7087edc8d2b334a78008d463c03.3.2.16.1731(long param_1,int **param_2);
undefined8 ifio_97db0.1aa17b629d8d811f779685c31040194afdf.3.2.16.1731(int *param_1,int **param_2,int param_3);
ulong ifio_cddb3.e6162daeeff892e35e6da791cc47ca500eb.3.2.16.1731(int *param_1,int *param_2,int param_3,undefined4 param_4,int **param_5);
ulong ifio_5928d.66c71edb114477a60ae895eb5aa0aa0e738.3.2.16.1731(int *param_1,int **param_2,char param_3);
void ifio_33af4.0dbd0762e37ae8875a93eba2c614f588acf.3.2.16.1731(int *param_1,int **param_2,char param_3);
undefined8 ifio_0a7fb.120bb15052072a215ccdf27b3ddd5309b1b.3.2.16.1731(uint *param_1,long param_2,ushort param_3);
undefined8 ifio_cd2c2.aa5cebbd151d91f6bbd1f02160fb8a0c12e.3.2.16.1731(long param_1,long param_2,int param_3,short param_4);
void ifio_aaca9.dd3652ed72a5df6668856c956ef59c8efcf.3.2.16.1731(void);
void ifio_1255e.b6340a9ffb0060b920e75621ccb014dafcf.3.2.16.1731(undefined8 param_1,long param_2);
void ifio_16b30.efee8b0ee2d1306c512dfad14152552e01a.3.2.16.1731(int *param_1,int *param_2);
undefined8 ifio_9510e.86c9cbce2857162b1d8be817486e2264b00.3.2.16.1731(int *param_1,int *param_2,ulong param_3);
undefined8 ifio_c2a63.23380fc4a6be5783b81f1b15f47b422f7ca.3.2.16.1731(long param_1,int *param_2,int param_3);
undefined8 ifio_7164d.a204983bc32b6960c94700908b3b6ee85d6.3.2.16.1731(long param_1,int *param_2,long **param_3);
undefined8 ifio_c1962.6cf2c62e61634e232ebe6f94c0e47024e84.3.2.16.1731(long param_1,long param_2,long **param_3,int param_4);
undefined8 ifio_92500.a7466b00c7328041446a5d1c792d5ce45e1.3.2.16.1731(long param_1,long param_2,long **param_3,int param_4);
void ifio_6e9f6.97a0b83245085581a8df03935e5c8a73d06.3.2.16.1731(long param_1,int *param_2,int param_3);
undefined8 ifio_0de53.dd270b3ea51b75a092f74f9cd1d0126b0f8.3.2.16.1731(int *param_1,int *param_2,int param_3);
void ifio_b7ba4.b9728f146f7b106a2b750b80377a6ced676.3.2.16.1731(int *param_1,int *param_2,int param_3);
undefined8 ifio_89238.fced407344c265a2d57de24474e48882ddb.3.2.16.1731(int *param_1,int *param_2,int param_3);
bool ifio_2db56.37641ce2cc1bfd349041f582613bd399fda.3.2.16.1731(undefined8 param_1,long param_2,int param_3);
int ifio_94a90.7bb0c78d84ae15487b4c70e39b7d9b8d5e3.3.2.16.1731(undefined8 param_1,int *param_2,int param_3);
undefined8 ifio_83d46.b5e46aa41088a449dc608abb84e97ddd855.3.2.16.1731(long param_1,int *param_2);
ulong ifio_8f25b.6e225d999c1d27be306bc93f0e5049f568a.3.2.16.1731(uint *param_1,int *param_2,ushort param_3,uint param_4,uint param_5,ulong *param_6);
void ifio_5a33a.f103642c1faef9d04c494756675325c1c47.3.2.16.1731(int *param_1,int **param_2);
bool ifio_1abda.abf99009b035a211e9dcffbaad5539b3cfc.3.2.16.1731(undefined8 param_1,long param_2);
void ifio_83bbd.fb2ab45c48666995d0a4e0991cd8418c104.3.2.16.1731(int *param_1,int *param_2,int param_3);
void ifio_6d7ac.d9c0e5c062834a4085d5bbdb4fe3c716200.3.2.16.1731(int *param_1,int *param_2,int param_3);
void ifio_4ac18.2e38d4512e72922d5c3f551a1e008cd72e7.3.2.16.1731(long param_1,int *param_2);
undefined8 ifio_8cad5.418cf0af000ffb597df03ba004010fcb0aa.3.2.16.1731(long param_1,int *param_2,ulong *param_3,int param_4);
void FUN_001b1150(long param_1,long param_2);
undefined8 FUN_001b1250(long param_1,long param_2);
ulong FUN_001b1360(int *param_1);
int ifio_48342.0651710475e57668dd7a1431c2bae1badc0.3.2.16.1731(long param_1,long param_2);
ulong ifio_41170.7cddaa556f4fb1f671df5d1a87c058d0eb0.3.2.16.1731(long param_1,long param_2);
void ifio_0557c.3f60530fd11601b318a00eaf7a207c5dd3b.3.2.16.1731(int *param_1);
ulong ifio_00122.a840b5d7efe1beb3dd8908ae37fe03eac74.3.2.16.1731(uint *param_1);
int ifio_7fe8c.819784dfaefc967517247b0bbeef82a6b7b.3.2.16.1731(int *param_1,long *param_2);
void ifio_88f0d.ef0737493813b57964c232d7d6b98d652cc.3.2.16.1731(long param_1,undefined *param_2);
int ifio_5cf9f.96602ac8a60d1cee10ee7c4916458f6274e.3.2.16.1731(int *param_1,long *param_2);
undefined8 ifio_500c7.15f5d3586fe9c7ccb469a2280b93195121a.3.2.16.1731(void);
undefined8 ifio_b0b98.b9099cfd7222e80a9f7565ccbe86b622f53.3.2.16.1731(long param_1);
undefined8 ifio_90c83.01ffd511f2b2bda73fd404b2a703473bc69.3.2.16.1731(undefined8 *param_1,undefined8 param_2,int param_3,undefined8 param_4,undefined8 param_5);
undefined8 ifio_89714.69e2f819c137c4e372c2dcd1f8a29d7ebfd.3.2.16.1731(long param_1);
undefined4 ifio_c1545.b0688c418bfb152a608d8cc104f8daf7df7.3.2.16.1731(long param_1);
void ifio_7fbc2.629c23a3faf738741b2a2afe9cdcf8f212d.3.2.16.1731(long param_1,undefined8 param_2,undefined8 param_3);
long ifio_3f2e2.611b9969f908d1b7cbe2bfd75796b473e33.3.2.16.1731(long param_1);
undefined8 ifio_98c9b.a5e71629190c45fec69899fe722de0d1fb9.3.2.16.1731(long param_1);
undefined8 ifio_e8b4e.f394aa4985de0fdc9a5a17c9f14e9a0e2e3.3.2.16.1731(long param_1);
void ifio_6c585.d99fc172dfb6fd2e3642b063612a209a7c2.3.2.16.1731(void);
void ifio_09b51.b75082896df21fc09e5f81e03dd14be8ac6.3.2.16.1731(long param_1,long param_2,uint param_3);
int ifio_a71d8.d423cf14ad2973bb287fb484bbfafb98604.3.2.16.1731(int *param_1,uint param_2,undefined *param_3,undefined8 param_4);
undefined8 ifio_a6789.9715785e21e73c5b824a49e6041973ef70a.3.2.16.1731(long param_1,long param_2,uint *param_3);
undefined8 ifio_110ca.d6dc27500d28d20e9648fbd5abb42af389d.3.2.16.1731(long param_1,long param_2);
void ifio_53d0a.b9d18c8377a87430c3fb7fd07279165491b.3.2.16.1731(uint *param_1,uint param_2,uint param_3);
void ifio_d849c.abde305bbbd55ec4178f23d7fa1b733ee66.3.2.16.1731(void);
void ifio_8c8a1.e9ce87eadc2e65dd040e7bd9bfa74e9a5dc.3.2.16.1731(long param_1,long param_2,uint param_3);
void FUN_001b2c70(int param_1,undefined4 *param_2,undefined8 *param_3);
ulong FUN_001b2ce0(long *param_1,long param_2,long param_3);
int FUN_001b2d30(long param_1,int *param_2,long param_3,long param_4,uint param_5);
void ifio_a76ab.91efba9f38d822f7c24e0bf4eccb359d785.3.2.16.1731(int *param_1,int *param_2);
ulong ifio_b7fc7.1fd08dfffbe4a7008b79d36cc17a29bdacf.3.2.16.1731(uint *param_1,ulong param_2,ulong param_3,ulong param_4,long param_5,long *param_6,undefined8 param_7);
int ifio_1942a.56a0cee239a571d520e8a71bcc9185dda61.3.2.16.1731(long *param_1);
void ifio_cdeac.23d008aa3a3607b0b4611c2c4069581ac2f.3.2.16.1731(long param_1);
long * ifio_6afe3.72703a4bf54a0049191f6ef9e6971a3f14f.3.2.16.1731(long *param_1);
void ifio_7c8c9.025ecaf882b32cafe9771714c5fd6a1d7aa.3.2.16.1731(long param_1,long param_2);
ulong FUN_001b4ad0(long param_1,long **param_2,uint param_3,int param_4);
ulong FUN_001b4c10(long param_1,long **param_2,uint param_3,int param_4);
int FUN_001b53d0(long param_1,long **param_2,uint param_3,ulong param_4,byte param_5);
void ifio_e092a.7de6e488e973ece8468e46b8b1f06fef8ca.3.2.16.1731(undefined4 *param_1);
int ifio_7f148.5c785c7be4d7c2701aeb89110cb9799d9b8.3.2.16.1731(long *param_1,ulong param_2,undefined param_3,undefined param_4,undefined param_5,undefined param_6,undefined4 param_7,undefined8 param_8);
void ifio_91136.cfedf319197b19918a78feaf82fdd166288.3.2.16.1731(long **param_1);
undefined8 ifio_75ca3.0c7e81c6c2c546e0be1ecbe860af80aec01.3.2.16.1731(long *param_1,ulong param_2,long *param_3);
void ifio_8e324.a51b52e395e38b313b2bb1442060168180b.3.2.16.1731(long *param_1,ulong param_2,int param_3);
bool ifio_7af6b.fda5517106671b5841ea667067956ec203e.3.2.16.1731(long *param_1,ulong param_2);
undefined8 ifio_32f86.eab9a3da02e56d54c94b11e44cc8a5b0b17.3.2.16.1731(long *param_1,ulong param_2,uint param_3);
undefined8 ifio_a4572.f399c73a7f79a44f3d41f4fbc7dc4b04d30.3.2.16.1731(long *param_1,ulong param_2,uint param_3);
undefined8 ifio_4c3a7.4bbf143334e8320237bec59e74e701abf08.3.2.16.1731(long *param_1,ulong param_2,uint param_3);
undefined8 ifio_6aef1.96712329b2b90b1391e9369c6d654eb0001.3.2.16.1731(long *param_1,ulong param_2,uint param_3);
ulong ifio_06fd0.27e9091685870e218464200e5b34b71070a.3.2.16.1731(long *param_1,ulong param_2,ulong param_3);
long ifio_a8e59.c5fbda7adc1269e869c985be5c31d7e2a72.3.2.16.1731(long *param_1,ulong param_2,long param_3);
long ifio_49c36.7933d4c176f9617b5e178ffeb162988606a.3.2.16.1731(long param_1,ushort param_2,ulong param_3);
void ifio_fc90e.cf25aa96a612f355e0d80c4ebeceabb92e7.3.2.16.1731(long **param_1,long *param_2,ushort param_3,uint param_4,int param_5);
undefined8 ifio_0256a.f620c632effb6ce36919b47421dce1aa4f7.3.2.16.1731(long *param_1,ushort param_2);
int ifio_e952e.242a90a207a675cc314b62d84d56d7c318a.3.2.16.1731(long *param_1,ushort param_2,int param_3,uint param_4,uint *param_5);
undefined8 ifio_461d1.b951e326bea78b88075a4d23ed45b76b428.3.2.16.1731(void);
void ifio_76f16.5968867557a0f7d9aec897a2c6e1b70fc25.3.2.16.1731(undefined8 param_1,undefined8 param_2,undefined8 param_3);
void ifio_78dac.9aa9cc7a65c018048507c718aefa90329ce.3.2.16.1731(ulong param_1,ulong param_2,long param_3,ulong param_4);
void ifio_f23e3.bf72ab6b915cb0c540e32b97113795002e1.3.2.16.1731(long *param_1,long *param_2);
void ifio_821ce.ecfd03a37349cdbe949ebb3e1ab79764138.3.2.16.1731(long *param_1,long *param_2);
void ifio_4b2cb.cb76569fd76f1c458d4fea51cad0aa0d123.3.2.16.1731(long *param_1,long *param_2);
void ifio_ea326.0b1ae8e3675ee976e00ffdb09b84b2a18f2.3.2.16.1731(long *param_1,long *param_2);
long ifio_7cbe7.aa7dfe6698814d7c8b13b3103658e6fe29a.3.2.16.1731(long *param_1,uint param_2,uint param_3);
undefined8 ifio_4c7c8.1396f1887909ace43572b4054c414c1ea24.3.2.16.1731(long *param_1,undefined2 param_2,undefined8 param_3);
undefined8 ifio_5ff4c.27a2674ffbf30cdce2d3effa09ac1c3d6d0.3.2.16.1731(long *param_1,undefined2 param_2,undefined8 param_3);
void ifio_41dfe.182bfee7eda1b0327989e92992bf1239dc1.3.2.16.1731(int param_1,int param_2,int *param_3,int *param_4,int *param_5,int *param_6);
int ifio_ea096.7402c3816b3fda38e6f9010cfb36137801d.3.2.16.1731(long param_1,int param_2,byte *param_3,int param_4,byte param_5,char param_6);
int ifio_1b781.6a1a0f80ab2d2b2f4773562cf6fb958f62e.3.2.16.1731(long param_1,int param_2,byte *param_3,int param_4,byte param_5,char param_6,long param_7);
void ifio_3ccfb.6deda863ceb3d8eef2e90cecea2f3a5c5e4.3.2.16.1731(long param_1,uint param_2,int param_3,long param_4);
void ifio_37e7d.e4abef7bf83feae7a1353b7a5fac86669eb.3.2.16.1731(long param_1,uint param_2,uint param_3,uint param_4,long param_5);
void ifio_9f295.f198c33f3b16f159aeeecf5208b4c6a38b4.3.2.16.1731(long param_1,uint param_2,uint param_3,long param_4);
void ifio_b01cb.5a07cd773407e6baf186e29baef3d70f796.3.2.16.1731(long param_1,uint param_2,uint param_3,uint param_4,long param_5);
int ifio_7bd4d.f2c89cd34a5eb06f4532b1a3976eb4811c8.3.2.16.1731(long *param_1,ushort param_2,uint param_3,uint param_4,uint param_5);
ulong ifio_978c0.a414b55401fb0439503292c541cd4bad9d9.3.2.16.1731(long param_1,long *param_2);
uint ifio_e0c1d.2f5cb3b8a873539588ae5cc0967f3cf0123.3.2.16.1731(undefined8 param_1,uint param_2);
undefined8 ifio_ee28e.2e694697aed48adc5cf7ca35c9126c0d996.3.2.16.1731(long *param_1);
void ifio_fce6b.66479942e9c359942a3bbaa1838faa6a219.3.2.16.1731(long param_1);
void ifio_fc83e.12a93855b72b9f790900bc30a11cd7712e7.3.2.16.1731(long param_1);
undefined8 FUN_001b9360(long *param_1);
undefined8 ifio_eeb7e.d4bf19625258354c8b6ee668c841e06a182.3.2.16.1731(long *param_1);
void ifio_650d2.3c3141ad83957737e8550fcd5edcd6f373e.3.2.16.1731(long param_1);
void ifio_cf333.90d349bade280a503352d80c60f6e52a158.3.2.16.1731(long param_1);
undefined8 ifio_4dc3d.a918a3454a009996b80c29b7804bdef7f28.3.2.16.1731(long **param_1);
undefined8 ifio_16729.f0b8b4630fdc1ca8694a32f39ad54605345.3.2.16.1731(long **param_1);
void ifio_9034c.dc6b5c732c3c49620b1a86479f1cf94797c.3.2.16.1731(long **param_1,long *param_2,ushort param_3,ushort param_4);
undefined8 ifio_1fa40.afc29198aaff32177fb04b685b9bfc3397b.3.2.16.1731(void);
undefined8 ifio_ff5e4.2b8723bea4166fa09bdd7db3c8146a753d8.3.2.16.1731(void);
undefined8 ifio_181c3.6349f106fd44d5d6da93706c25dcc8817ec.3.2.16.1731(long **param_1,ushort param_2);
undefined8 ifio_f864c.d91b0eb7ca963c03e39b96ee8a06e0f5bc3.3.2.16.1731(long **param_1);
undefined8 ifio_5653d.d7725c09108e66cc98152edb696b4e077b6.3.2.16.1731(long param_1,long *param_2);
void ifio_72afa.9efd9d8cd20e431af6ac32df3c2982aa15b.3.2.16.1731(long param_1,long *param_2);
void ifio_6fc66.30447c947b820b6b38b82b3102663aa5e19.3.2.16.1731(long param_1);
void ifio_fced6.a8b2c49560c689f1c1d5c6c6f705c9b23a3.3.2.16.1731(long *param_1);
undefined8 ifio_79c45.e27c6f5dfe3b6c010ae05d1b10f6e00b6ba.3.2.16.1731(long *param_1,undefined8 param_2);
ulong ifio_357ba.6fe203b96e4513a0d37dfc5efe4e0ac0338.3.2.16.1731(long *param_1,char param_2,undefined8 param_3,undefined4 *param_4);
undefined4 FUN_001baef0(long *param_1,ushort param_2);
void FUN_001bb0d0(long param_1,char *param_2);
undefined8 ifio_c9342.4e7ce3db5894c8c87e753ea748e7144ea5f.3.2.16.1731(long param_1,ushort *param_2,char *param_3,undefined4 *param_4);
int FUN_001bb2b0(long *param_1,ushort param_2,char param_3);
undefined8 ifio_b84e6.fb31982b7b84dd8a1652c8d0bf9b693dda9.3.2.16.1731(long *param_1,long param_2,ushort *param_3);
undefined8 FUN_001bb9c0(long *param_1);
undefined8 ifio_b3419.dba085cb945c03228a9f7781fab8a790237.3.2.16.1731(long param_1);
undefined8 ifio_8dd0b.7bb29cc5d446a0cf62ebbc6e1f87f100c3b.3.2.16.1731(long param_1,uint *param_2);
undefined4 ifio_42229.e54b15f4172d2260d5ee4bb9e12f7f2154b.3.2.16.1731(long *param_1,long param_2);
undefined8 ifio_f4c50.c0d1a7bfe745ba4e721a3b603ea1297047e.3.2.16.1731(void);
undefined8 ifio_ef9b5.aabda26d01e27f79b81c62814f2bb5c05f6.3.2.16.1731(long *param_1);
undefined8 ifio_4fb21.e8b6644d5603f5df0cfed8f5b5effa69bce.3.2.16.1731(void);
ulong FUN_001bd700(long param_1,uint param_2,long param_3,long param_4);
ulong ifio_060d7.926570f1ad222103e8a8df7d6137b658228.3.2.16.1731(long param_1);
undefined8 ifio_c901c.9679252dffaf60d08de12712c3822e5bc15.3.2.16.1731(long param_1);
void ifio_147cd.56c4e53011458d1d4e76d8d6d841143361d.3.2.16.1731(long *param_1);
void ifio_f27d8.1ccab5b57ba1fece4e89d59ea8d9cca3f79.3.2.16.1731(long *param_1);
undefined4 ifio_72f9b.e53328dce498e23a7d0a4a52028729b7273.3.2.16.1731(long param_1);
int ifio_17743.b6f25721249b4e8c63bae9d10473a020990.3.2.16.1731(long param_1);
void ifio_b5777.c6702b173e214655ee18c0721afabfa7dfb.3.2.16.1731(long param_1,int *param_2,int *param_3,undefined4 *param_4);
void ifio_3e9ee.0f5a18b06cdb2b34056570bf2584147463f.3.2.16.1731(long *param_1,ushort param_2);
void ifio_a5030.3f61f0bb4ee482dc3bdf731bf3a7c34ce59.3.2.16.1731(long param_1,ulong param_2);
void ifio_50a5b.f8493cb9f8eba57d3f26dc67f98077341ab.3.2.16.1731(long param_1,uint param_2);
ulong ifio_e5f3f.49708a392b3802254f5611faa9ec293fd43.3.2.16.1731(long *param_1,ushort param_2);
void ifio_edadb.da4d5939b0a96c6d8f577b780c991a69c24.3.2.16.1731(long param_1,int param_2);
undefined8 ifio_faafe.0ae86a44d2222e74920fcce465ee77ea171.3.2.16.1731(long *param_1,long param_2,uint *param_3,int param_4,char param_5);
undefined8 ifio_f83ee.fb08e9f8058d9908ee7f6941f278ad066ed.3.2.16.1731(long *param_1,char param_2);
void ifio_ec788.273d417fa42e94cb63e1929e94e49cf4c33.3.2.16.1731(long *param_1,ushort param_2);
ulong ifio_200f0.ad1d8d12a47c41d659d59a7a0775c8552f8.3.2.16.1731(long *param_1);
undefined8 ifio_b535e.0ea13c266260052b5ebb63ab44436a60956.3.2.16.1731(long *param_1);
undefined8 ifio_11f04.3e1858c34b1272380c18380987dbf740fcf.3.2.16.1731(long *param_1,ushort param_2,ushort param_3,uint param_4,char param_5);
void ifio_64685.4a2186c6a862286b0ae931db6ae1caa627e.3.2.16.1731(long **param_1,ushort param_2,uint param_3,uint param_4);
void ifio_c609b.54030b9a98be79ba89e545176cc6aaa08fb.3.2.16.1731(long **param_1,ushort param_2,char param_3,uint param_4);
void FUN_001bf7e0(ushort param_1,long param_2,undefined8 param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6,undefined param_7);
undefined8 FUN_001bf840(long param_1,short *param_2,char *param_3,undefined4 *param_4,undefined8 param_5,undefined8 param_6);
undefined8 FUN_001bfcc0(long param_1);
void ifio_dda47.c1b5235283c58b9f6157a1f05f0e31ca2b8.3.2.16.1731(void);
undefined8 ifio_2a9d3.be9e7bbf5ab06e60a3926280e1a6f73bfea.3.2.16.1731(long param_1,undefined8 param_2);
ulong ifio_57b69.9b5f1c5690cba484f513845200884b5141d.3.2.16.1731(long *param_1);
ulong ifio_f52d0.a1c7a707facf2bf10ca82581de0be408cde.3.2.16.1731(long *param_1);
undefined8 ifio_6ac43.cb2fcf757ba0e6beca55d7251689f618055.3.2.16.1731(long **param_1);
ulong ifio_97ec4.37875990dd3abb601e864877a3fee8cbfcd.3.2.16.1731(long **param_1,undefined8 param_2,long *param_3,long *param_4,undefined8 param_5,long *param_6);
undefined8 ifio_cf464.dccd4cc887c69d0e2e546e949982c886426.3.2.16.1731(long *param_1,undefined4 param_2,undefined8 param_3,undefined4 param_4);
void ifio_a3d3c.33f32ad09b2a6cf962ee435d0b46c84d4b0.3.2.16.1731(long **param_1,long param_2);
int ifio_c6e99.5bf0dfb0d9c79b652f43e0dbb934b5a4c13.3.2.16.1731(long *param_1,ushort param_2);
long ifio_98ad0.ec1a5177addcb8222d0468ada3408a2f6c7.3.2.16.1731(long *param_1);
long ifio_e9494.c1646e272265878e901de61faa322b38c89.3.2.16.1731(long param_1,int param_2);
undefined8 ifio_34d76.6a6739a6e34559d702e86eaa5cde3b46562.3.2.16.1731(long *param_1,ushort param_2,ushort param_3,long param_4);
bool ifio_19b0e.adf2c35263aa4e2f502de4873d7a3c3e463.3.2.16.1731(long param_1,ushort param_2,ushort param_3);
void FUN_001c0d40(long param_1);
undefined8 FUN_001c0d90(long **param_1,int param_2,long *param_3);
uint ifio_5e9da.143addb86ea69b44fd519383e3f2a2a285c.3.2.16.1731(long param_1);
void ifio_0e45b.a95f770cc8f9d878f5b36d75510b1d27971.3.2.16.1731(long param_1,byte *param_2);
undefined8 ifio_cf372.d20c4402545f9892e16668c09d399a85718.3.2.16.1731(long **param_1,ushort param_2,int *param_3);
undefined8 ifio_9d162.37f2edd5a0f464dd4c27739ae227683b580.3.2.16.1731(long **param_1,long *param_2);
undefined8 ifio_e45a7.e7a9d241bd14f206d846e536af53913c287.3.2.16.1731(long **param_1);
int ifio_accda.348e6e74a412e8b9de5764c9e6b82f27a93.3.2.16.1731(long **param_1,long param_2,long param_3);
int ifio_5e8c3.e5558d5552a6aded0bf998c07b1d72089cc.3.2.16.1731(long **param_1,uint param_2);
int ifio_5eca7.7ac603f27694b0fa1db017a0d4fcb6f7726.3.2.16.1731(long **param_1,byte param_2);
int FUN_001c2260(long **param_1,byte *param_2);
int ifio_8335c.0c440d74b3bfcfa7c041c4221e7aabfd371.3.2.16.1731(long **param_1,uint param_2);
int ifio_233fe.c5d837e2335a3d5e4b32785df2afa399ba2.3.2.16.1731(long **param_1,ulong *param_2,undefined4 param_3);
int ifio_9dcb2.15c2014e6146a0904aa20a1b83c3f152fa8.3.2.16.1731(long **param_1,ulong *param_2,long param_3,byte *param_4,int *param_5);
int ifio_1aebd.ff59408fc567fcbcadb26cb7c4d066819e7.3.2.16.1731(long param_1,long *param_2);
void ifio_dc053.a12e067c57b8f0635c0a70c30317999afd5.3.2.16.1731(long **param_1,byte *param_2,int *param_3);
void ifio_752c1.249a9f486b1881b49eb0968eda0ba517845.3.2.16.1731(long param_1,undefined8 param_2,int param_3,int param_4);
void ifio_43625.8633e1b6355f62232c099531b5841b7d842.3.2.16.1731(void);
undefined8 ifio_4ddf4.4c85ba5d47d32529711bd98ee35738269aa.3.2.16.1731(long param_1,ushort *param_2,undefined4 *param_3);
undefined  [16]ifio_2bd50.3374920b30639205a25f75d416ad08fe35c.3.2.16.1731(long *param_1,ulong param_2,ulong param_3);
void ifio_54763.e53634226524173d9b47029b42f9d991e33.3.2.16.1731(undefined8 *param_1,undefined8 param_2,undefined8 *param_3);
void ifio_5bd0a.ef75a678a576c17d7f2e23f0201b1d72fa0.3.2.16.1731(long *param_1);
void ifio_e7d2f.4f6df1744d6fd63620cd19abe16b5079fa9.3.2.16.1731(long param_1,ushort param_2,ushort param_3);
undefined4 ifio_ad26d.8f7cab76fd697345b1c902a7e3b6d7909d7.3.2.16.1731(long *param_1);
void ifio_3b5e9.04186d3f9a4fe5e7247ee67c10685e308f3.3.2.16.1731(long param_1,ushort param_2);
void ifio_acb74.3f524e52e43775b7112130fdfbfc72326e6.3.2.16.1731(void);
void ifio_9306f.0089cb1619f2b32b367baa78b225d1f3163.3.2.16.1731(long param_1,ulong param_2,undefined8 *param_3);
uint ifio_565d7.656a92c6cb65b571b08193d39345727bdd2.3.2.16.1731(byte *param_1,int param_2);
void ifio_f3ad4.0696b73810d1835229edcedd4570e33fe3a.3.2.16.1731(long param_1);
void ifio_f41c6.c211c0349ed1ef0f3b7ca3def2c01c6c523.3.2.16.1731(void);
void ifio_85475.6c3c6469bbe01fe8330bfc649251e830b5a.3.2.16.1731(void);
undefined8 ifio_9ae0a.6a5055d3234f3a7a37417d888c49a4cfc46.3.2.16.1731(undefined8 *param_1);
undefined8 ifio_e4cb6.ee3f43f8d1e7679cc24b47fa43b067de615.3.2.16.1731(long *param_1);
undefined8 ifio_f1e2a.acdf83789fbdda6d3c0b360e23d47bf9be1.3.2.16.1731(long param_1);
ulong ifio_1856d.a96526d60fa232c4be0420f2fce4fe7cb56.3.2.16.1731(long *param_1,undefined8 param_2,int param_3,int param_4);
int ifio_7a28c.386717efff7d425658023672660a01c6473.3.2.16.1731(long **param_1,long *param_2,undefined8 param_3,long *param_4,int param_5);
undefined8 ifio_dfb78.15cb878b2257af4bf0b533b4d9b63aede1e.3.2.16.1731(long param_1,long param_2);
undefined4 ifio_32ba8.d4769f50f18cbe49564226f936353f5271f.3.2.16.1731(long param_1,long *param_2,long param_3);
void ifio_bf3be.71af47769de63fb8299d192eea6e726e03c.3.2.16.1731(long param_1);
void ifio_d007b.0092677e3235a5a29deb39b930ff1337e95.3.2.16.1731(undefined8 param_1,long param_2);
void ifio_3e976.d4c9d3e69e3d81699c909620b7921a64092.3.2.16.1731(long param_1,int param_2);
void ifio_1c7d9.9be939e190c15a8f0a2c6501f0865598abf.3.2.16.1731(long param_1);
undefined4 ifio_1ff7e.c72914f962a4f555cb7aea3ef627de43936.3.2.16.1731(long param_1);
undefined8 ifio_ca42b.2ed104c5cbe5c5befa6c5cb6258bba642c2.3.2.16.1731(long param_1);
undefined8 ifio_9cff4.f4a7c3638de0716c8b1dca437d2e5482732.3.2.16.1731(long param_1);
undefined8 ifio_a2156.d5c93e0e2e21e625759c4ea875a33b02f36.3.2.16.1731(undefined8 *param_1,undefined8 param_2,undefined4 param_3);
void ifio_e0d1b.40336a782e9f7914cb146849a97f21adf6a.3.2.16.1731(undefined8 param_1,long param_2,undefined8 *param_3);
undefined8 ifio_26eca.3cc8da6640dc96542685dc013958d034edc.3.2.16.1731(undefined8 *param_1);
int ifio_18ff4.c3b75a952801043a1f328220b5d24398482.3.2.16.1731(long **param_1,undefined8 *param_2);
undefined8 ifio_66dfe.1e94e743561cd8c0ef9f35e0062977dd3af.3.2.16.1731(long param_1,undefined *param_2,undefined8 param_3);
int ifio_10c07.1804d31df6677c290654b02d799d9578018.3.2.16.1731(long *param_1,undefined8 param_2,ulong **param_3,ulong *param_4,ulong *param_5,ulong **param_6);
undefined8 ifio_dfb66.29a9c43f7b4941ca6379366a0d5315de9ec.3.2.16.1731(long param_1,undefined8 param_2,long param_3);
undefined8 ifio_20f75.f918ea7dcef66c5fd057e5725b901c724fd.3.2.16.1731(long *param_1,long *param_2);
undefined8 ifio_55102.0d46a01cb6b22055712dd37941ed7b3dbb1.3.2.16.1731(long *param_1,long *param_2);
undefined8 ifio_59457.87f2abdf3011cd1b6298e0cafb3da3f7938.3.2.16.1731(long *param_1);
undefined8 ifio_b1a5f.2c93aa3817c38bd516bc9a993a1f3106cc9.3.2.16.1731(long param_1,long param_2,undefined8 *param_3);
void ifio_2ed78.fd7650b8415b367a2560b3f04b04887ca65.3.2.16.1731(void);
undefined8 ifio_1dd3c.ddf6ff998f027cb4c8ad18f47c328175f6c.3.2.16.1731(void);
void ifio_98c7a.91bbffade18a76e6fe8b93cae4c9e9ef8cb.3.2.16.1731(undefined8 *param_1,undefined8 param_2,undefined8 param_3);
void ifio_f085f.3a0127ebcce98a97ffc012329127ef7941d.3.2.16.1731(long *param_1);
long ifio_bd9cf.84526194ee9d05add61e2490126117e63f9.3.2.16.1731(long param_1);
undefined8 ifio_68527.bad7d016b1b62b7314ecceabd97d6e06448.3.2.16.1731(long *param_1,long param_2,int param_3);
int ifio_69b5d.8f0a3af7555f33bfc0328af7c8a1fd77e5b.3.2.16.1731(long **param_1,long *param_2,int param_3);
undefined8 ifio_3217e.1800fd927f645310d2ded5530e64151b171.3.2.16.1731(long *param_1,long *param_2);
void init_fio_obj(void);
void cleanup_fio_obj(void);
void ifio_bf788.b89eec5d20d1db6b0801614411685473d2d.3.2.16.1731(long *param_1,ushort param_2,ulong param_3,long param_4,uint param_5,long *param_6);
void ifio_edd61.afd80f90d214efb7d31f37fc91d86618b4c.3.2.16.1731(long *param_1,undefined8 param_2,int *param_3,int *param_4,uint *param_5,undefined8 *param_6,undefined8 *param_7,undefined8 param_8);
undefined8 ifio_6c724.68f63c82f7198a72326a8d0ec2c9cca5eee.3.2.16.1731(long *param_1,long *param_2,undefined2 param_3,int param_4,int param_5);
undefined8 ifio_0fce4.dc030279667b379a9e22b4ee7dc37df90c3.3.2.16.1731(long param_1,long *param_2,undefined2 param_3,int param_4,int param_5);
void ifio_55297.39038087621c7dd56efab1b4c81244774fd.3.2.16.1731(void);
void ifio_ee735.a4d1ad641dd3894e4b324ac540d46631aa6.3.2.16.1731(long *param_1,ushort param_2);
ulong FUN_001c6cb0(long param_1,long **param_2,uint param_3,int param_4);
ulong FUN_001c6df0(long param_1,long **param_2,uint param_3,int param_4);
int FUN_001c75b0(long param_1,long **param_2,uint param_3,ulong param_4,byte param_5);
int ifio_253aa.d9f26a91b6f75803acdf6686acdfc89de14.3.2.16.1731(long **param_1,ulong param_2,long param_3);
void ifio_ef3ca.2a614e17e5ce2c2070c5f5dced4528dd001.3.2.16.1731(long **param_1);
void ifio_bb3c5.57c9604dd15ac4eb91683718581f575d877.3.2.16.1731(long *param_1,uint param_2);
void ifio_fc38c.671ee2bdbf237c204586689563b5106e388.3.2.16.1731(long *param_1,uint param_2,int param_3,ushort param_4,int param_5);
ulong ifio_81f28.71af377db3e797ed7d8d1e5b6f5cdd34995.3.2.16.1731(long *param_1,ulong param_2,long param_3);
int ifio_b21bf.e84472ab1fc93f1d71dede20b98f312dac4.3.2.16.1731(long *param_1,ulong param_2,long param_3,ulong param_4);
void ifio_86156.9e3f9495c3df7014f86f73c612cd6375ccc.3.2.16.1731(long param_1,undefined8 *param_2,undefined8 *param_3);
int ifio_eac3f.b8b38e3352e44e5f707a1da9641f5a5c765.3.2.16.1731(undefined8 *param_1,long param_2,int param_3,undefined8 param_4);
undefined8 ifio_9e41f.3fda1da17a97c09cbfbf9e76c0ec30f056e.3.2.16.1731(ulong **param_1,char param_2);
undefined8 ifio_a6aaa.838bdff9a56e893b99b89a5a80a0732338e.3.2.16.1731(long *param_1,uint param_2,int param_3,int *param_4,int param_5);
ulong FUN_001c8d30(long param_1,long **param_2,uint param_3,int param_4);
ulong FUN_001c8e70(long param_1,long **param_2,uint param_3,int param_4);
int FUN_001c9630(long param_1,long **param_2,uint param_3,ulong param_4,byte param_5);
int ifio_95722.caf18282769b6a2be768f60e854be0f7ebc.3.2.16.1731(long param_1,int param_2);
void ifio_cdd2d.88948386647d46d03c4c55d1330c2bdfede.3.2.16.1731(long **param_1);
int ifio_6eabf.6412a262f056afa23e58b99a53cd78de537.3.2.16.1731(long **param_1,uint param_2,long *param_3,long param_4);
void ifio_d9e93.ee9d2ba40813d6f4fb1ce7609f46d6f761c.3.2.16.1731(long param_1,uint param_2,ulong param_3);
void ifio_9133f.bd3c78088237f09fcf64ffa52fb2d733b60.3.2.16.1731(long param_1,undefined8 *param_2,undefined8 *param_3);
undefined8 ifio_97943.577a5c1563801f74d56d577947507f769c7.3.2.16.1731(long param_1);
void ifio_08cc1.9783e847e2480d87041cac5df37f1721b98.3.2.16.1731(void);
void ifio_c1a6d.62d26e6d9fbbf63581bb9a323de9e635d11.3.2.16.1731(void);
int ifio_3a86b.e53de8d94e9d1f21c9b77fe46eb37051c37.3.2.16.1731(long *param_1,ulong param_2,uint param_3,ulong param_4,uint param_5);
int ifio_01c3d.23e2ee88cfb42aa90639d87d09ea15a0eef.3.2.16.1731(long *param_1,ulong param_2,uint param_3,ulong param_4,uint param_5);
int ifio_8c827.f1e64c5d3e7bb4f685257e04fb4493a5edb.3.2.16.1731(long *param_1,ulong param_2,uint param_3,ulong param_4);
void ifio_8740c.3825a2d633143c5e007595d77eab7747c8e.3.2.16.1731(int *param_1);
void ifio_c8655.373e0a599e7e03de4fc07d3d35a73e455c1.3.2.16.1731(long *param_1,uint param_2,long param_3);
void ifio_7d6b7.46367ba338b2a8eb466a039a6bbdd54ddb4.3.2.16.1731(long param_1,uint param_2,ulong param_3);
void ifio_2efae.1ffcf87a057477b56e4acd54c018a29f60a.3.2.16.1731(long *param_1,ulong param_2);
ulong ifio_1ac33.024785fbe55db4e3b320e37d59f9dbea62f.3.2.16.1731(long param_1,ulong *param_2,uint param_3);
undefined8 ifio_b43dd.66cac96be215ee17eb7905879f0c3418a3c.3.2.16.1731(long param_1,uint param_2,uint param_3,long param_4,undefined8 param_5,int param_6);
ulong ifio_93176.a0449047e3c259b1b6bd6fc07b6056d86d2.3.2.16.1731(long *param_1,ulong param_2,ulong param_3,ulong param_4);
undefined8 ifio_fcc25.c0501ee3e41a09cd63d09f299edd5bc64cd.3.2.16.1731(long param_1,long param_2,long param_3,undefined8 param_4,int param_5);
undefined8 FUN_001cbaf0(long **param_1);
void ifio_db568.915d12c91a792fbd22ae6ccbaf3f3d4390e.3.2.16.1731(long param_1,ushort param_2,uint param_3);
undefined8 ifio_d6c0a.d8d34db6871f35129a3965a0d1e50d70767.3.2.16.1731(long *param_1);
void ifio_d76d0.c8a48a18b05257fbd616d45759b9fef565b.3.2.16.1731(long param_1);
void ifio_d636c.2eaeecb239fc49f4b07b88a776070b98ee4.3.2.16.1731(long param_1);
void FUN_001cc160(long *param_1);
void FUN_001cc170(long **param_1,long param_2,long param_3,ushort param_4,long param_5);
void FUN_001cc520(long **param_1,long param_2,long param_3,ushort param_4);
undefined2 ifio_e16c3.3d1a66c48d4227ad4bb5b99c0ccf9421602.3.2.16.1731(long param_1);
undefined8 ifio_6bb21.77e63c3a03f26cd641a68d2c799370e8601.3.2.16.1731(long *param_1);
void ifio_d2793.875d17e129f41ec206d02c3cfea36bcb9a7.3.2.16.1731(long param_1);
void ifio_034fd.5f32dd6d3117308997fe6c5d19ec507890f.3.2.16.1731(long param_1);
void ifio_5cf62.279186a5558c816b12cbd72cdf197be38ab.3.2.16.1731(long param_1,ushort param_2,int param_3,long *param_4);
undefined8 ifio_c5f26.5d0b9ac8a9598238322349deb964acc8ba5.3.2.16.1731(long param_1,ushort param_2);
undefined8 ifio_fdced.04996cd53f53f0026264fb774fffe264d67.3.2.16.1731(long *param_1,ushort param_2,uint param_3,uint param_4);
undefined8 ifio_97cf1.e9ad30d0ecf1bd7a72eec201aba696c0478.3.2.16.1731(long param_1,ushort param_2,uint param_3,uint param_4);
undefined8 ifio_7f85d.3c2044edb0b7c6a62407e4493e362219b66.3.2.16.1731(long *param_1,ushort param_2,uint param_3,uint param_4);
void ifio_ae224.bff5195420724b4e4194567e0fa96df8fc3.3.2.16.1731(long *param_1);
void ifio_363ca.b258c8bdfe8469be5e7060eb3baca933352.3.2.16.1731(long param_1,ushort param_2,uint param_3);
void ifio_23ed5.489297f3b315a5f9fd2eeb34b3011f9cc80.3.2.16.1731(long param_1,ushort param_2,ulong param_3,int param_4);
undefined8 ifio_c92b2.0219bf6cb843c7463b8b9a17dc71746ae4e.3.2.16.1731(long *param_1,int param_2,ushort param_3,uint param_4,uint param_5);
undefined8 ifio_b8496.0f4ede36d33ef2e160144760625999cd48d.3.2.16.1731(long param_1,ushort param_2,uint param_3,uint param_4);
int ifio_96cef.44fb80bed46325241a7d4e04d4493641634.3.2.16.1731(long param_1,ushort param_2,ulong param_3,uint param_4);
void ifio_d7ddf.c9c5a85920e045aa0704a281be5293faf10.3.2.16.1731(long param_1,ushort param_2,uint param_3,uint param_4);
ulong ifio_23ab8.f882afc959c2a194f50055ccb6abc2d0db4.3.2.16.1731(long param_1,ushort param_2,uint *param_3,uint param_4,uint param_5);
undefined  [16]ifio_28a56.713a7e976e789264eee9b733594b91df120.3.2.16.1731(undefined4 *param_1,undefined4 *param_2,undefined8 param_3);
undefined8 ifio_6fd5e.dc9dbe832a7f717f5f4e7789c0bdbb9a399.3.2.16.1731(long *param_1,int param_2,int param_3);
void ifio_b82ec.df3dd7dba99f699348405681039b6fc64d1.3.2.16.1731(long param_1);
void ifio_9cf7b.eea6115d156731be0909c312ccf05d3d0b2.3.2.16.1731(long *param_1);
undefined8 ifio_94b96.c655aff4e467d0b012e4f82229a4a7b0856.3.2.16.1731(long param_1,ulong param_2,long param_3,uint param_4);
uint ifio_77c89.0bf532c30e3280f4cfc3335c89481cd2a17.3.2.16.1731(long param_1,ulong *param_2,ulong param_3,ulong **param_4);
undefined8 ifio_4b70e.ae674212533e9d0dfb89df95475b90ad7e1.3.2.16.1731(long param_1,long param_2,ulong **param_3);
undefined8 ifio_f87f1.d981e9615e414d94a8c2034906a32570e0a.3.2.16.1731(void);
undefined8 ifio_99a98.154ae3489dc4e1134f5efd4d92e52448622.3.2.16.1731(long param_1,long param_2,uint param_3,uint *param_4);
void ifio_2539f.01406bb90dc8c24f964717995bdf793e641.3.2.16.1731(void);
void ifio_b43ad.9c73a71515e3f86e8c8268e6c57d203f483.3.2.16.1731(void);
undefined8 ifio_61169.2214bff1c66e2a24f8f52216a81c186e8c9.3.2.16.1731(long param_1,ulong param_2,undefined param_3,long param_4,uint param_5,int param_6);
undefined4 ifio_67db0.38d6f531f885305dd5cc065be6b274b9bee.3.2.16.1731(long param_1,ulong param_2,undefined *param_3,long param_4,uint param_5,undefined4 *param_6);
undefined4 ifio_2f83f.f29c4ca2a6938cc0c278406158097df54e4.3.2.16.1731(long param_1,ulong param_2,undefined *param_3,long param_4,uint param_5,undefined4 *param_6);
undefined8 ifio_3dbb5.f923646f1263ba3f6611168ba44e05cd502.3.2.16.1731(long param_1,undefined *param_2);
undefined8 FUN_001cfeb0(long param_1,long *param_2,ulong param_3);
undefined8 FUN_001cffc0(long param_1,long *param_2,long *param_3);
ulong FUN_001d00e0(long *param_1,undefined8 param_2,undefined8 param_3);
void ifio_b6f57.0cfc852f34b42c80e1c25995c5ed586909d.3.2.16.1731(long param_1);
char fio_pps_is_started(long param_1);
ulong ifio_19a8e.389611d2d3cb250a05db4d4f06943a18cab.3.2.16.1731(long param_1,undefined4 *param_2);
ulong fio_pps_format(long *param_1,long param_2);
ulong ifio_9a023.54db765f4eaf1be5026baa1e3683b6fc4e6.3.2.16.1731(long *param_1);
ulong ifio_efe12.44af714b126cb8322cb4c4426340b30bcfd.3.2.16.1731(long param_1,undefined8 param_2,undefined2 *param_3);
ulong ifio_2417e.7e5b4eaba4400e89fb75eb910cfbdcbaf94.3.2.16.1731(long *param_1,ulong param_2,ushort *param_3);
ulong ifio_ba7f5.9340d5a6a91633d69f21770a3f74643719a.3.2.16.1731(long *param_1,ushort param_2);
undefined4 ifio_328a2.608c13c8a45926031cbca989970e72a0b59.3.2.16.1731(long param_1,ushort param_2,long param_3);
int fio_pps_get_value(long *param_1,ushort *param_2,long **param_3,uint param_4,uint *param_5);
ulong fio_pps_set_value(long *param_1,ushort *param_2,long **param_3,ulong param_4);
int fio_pps_delete_value(long *param_1,ushort *param_2);
uint ifio_bf445.da6bea2a31b57f7a03206b280cf65b84009.3.2.16.1731(long param_1,uint *param_2);
uint ifio_985a9.699ebe064ca3e3854916f4b1104f5700a6b.3.2.16.1731(long param_1,int *param_2);
undefined1 *ifio_6602e.a864ad6eee3a834cee19f825aa82123e83f.3.2.16.1731(int param_1,int param_2,int param_3);
undefined8 ifio_ffb14.952899181dd4b49b810b653562e2da7b4d0.3.2.16.1731(long *param_1,int param_2,int param_3);
void ifio_afbd8.2407179b29b7d0754c60df534662da0b98d.3.2.16.1731(undefined8 *param_1);
void ifio_5520d.ae72d28072f88de4b0261ed39e64a426982.3.2.16.1731(long *param_1);
void ifio_977a0.51f6c2e738f2a0cfcd407cfa1bbdbd28b01.3.2.16.1731(void);
void ifio_b55cd.27843c68871c5b8d13f28f59ffc18d505ff.3.2.16.1731(void);
undefined4 ifio_770a9.e5c5a0ba78a0c9b4a262a5432a26bef270e.3.2.16.1731(undefined8 param_1,long param_2,int param_3,long param_4,undefined8 param_5);
undefined4 ifio_e0d34.a59d98dab473a04257b477e4a02c85e1ad4.3.2.16.1731(int *param_1,undefined8 param_2,undefined8 param_3);
undefined4 ifio_25801.fad1dd0bbcd84d326395bde409e9d162097.3.2.16.1731(int *param_1,undefined8 param_2,undefined8 param_3);
void ifio_73583.5e2b45eef2da202a089fbbbf622292648ba.3.2.16.1731(long param_1);
uint * ifio_17b2b.897fccf533304736a61c0dcbd906430aa1b.3.2.16.1731(uint *param_1,long *param_2,long *param_3,char param_4);
void ifio_bd63e.9b3e09347e4a87374d5b935c3253ef3b929.3.2.16.1731(long param_1,long param_2,ushort param_3);
void ifio_996e6.afb4c88fe755a7402998f9faf9619c6d7f4.3.2.16.1731(uint *param_1);
void ifio_9dc51.cf182cb3f5e4ead722e297993136be20d95.3.2.16.1731(uint *param_1);
void ifio_6f348.959e0675ed92311acb2de8d6b5b1a679a09.3.2.16.1731(uint *param_1);
void ifio_785d2.67a9fdb4bfd33cf24162c0571e4619684ee.3.2.16.1731(uint *param_1);
undefined8 ifio_19c26.d8a3db07cd6bbb78c289acc1295e4f02ffc.3.2.16.1731(void);
int ifio_4ea8a.3a350774b12b324c6bba1e1d2041fedf45d.3.2.16.1731(int *param_1,int **param_2,long *param_3,long *param_4);
undefined8 ifio_242ed.113f9ffe9c35cf9b130de0b73d85a515615.3.2.16.1731(long param_1,long **param_2,undefined8 *param_3);
void ifio_3cfb0.d598a16f42adca3e83dfca3c1a20f64647e.3.2.16.1731(long **param_1);
undefined8 ifio_179b2.484d75ed8320f86421d977a133b1371bee3.3.2.16.1731(int *param_1,long **param_2,long **param_3);
undefined8 ifio_25c74.3d95ac51ac28662ebd173e7c8ed32ae654c.3.2.16.1731(int *param_1,undefined8 param_2,long *param_3,long *param_4);
void ifio_bed39.522c258e9df258ac13c44d960213b62e8b2.3.2.16.1731(long **param_1);
undefined8 ifio_023c9.d63283edfa814b0eef933ccad0a608885f2.3.2.16.1731(undefined4 *param_1,undefined4 param_2,undefined4 param_3);
undefined2 ifio_faf57.f60c8ef2aca733a5989c3239ecd64e43700.3.2.16.1731(long param_1,short param_2,short param_3);
uint ifio_9d9ca.581d1cdeb3f8990b7d94a2c1ffe5cf2050b.3.2.16.1731(undefined8 param_1,uint param_2,uint param_3);
undefined2 ifio_5c93b.6ff3682bfd175e0878a9c2953908164e333.3.2.16.1731(long param_1,short param_2);
undefined2 ifio_11ca7.2a9067fddafdbdc0f054e97c1914b27f677.3.2.16.1731(long param_1,short param_2);
void ifio_1ccb0.dacb1473a1892bf5593a3c24a4b35fede40.3.2.16.1731(long param_1);
void ifio_b4d7c.c3c3bb4332e5aa9d8aad8d7b0d47d66fa09.3.2.16.1731(long **param_1);
undefined8 ifio_bea12.c9e0c9fcf472a09f8cd936f8345291686c3.3.2.16.1731(long param_1,undefined8 *param_2,undefined8 *param_3,long *param_4);
undefined8 ifio_9ea17.9876bab3dce64ad0d7d89e9fa706ad57062.3.2.16.1731(long param_1,long **param_2,long **param_3);
void ifio_9aac1.435d4d886e77ea4d400c425fd65e52c4b51.3.2.16.1731(long **param_1);
undefined8 ifio_8c1d4.5a2bf113ddf64c9e8ff257e9814fa423ab9.3.2.16.1731(long param_1,undefined8 *param_2,undefined8 *param_3,long *param_4);
undefined8 ifio_b73ac.1df48a170789f7b68e65ff3053f1ed81d0e.3.2.16.1731(long param_1,long **param_2);
void FUN_001d4120(long **param_1);
undefined8 FUN_001d4170(long param_1,short **param_2,undefined8 *param_3,long *param_4);
undefined8 FUN_001d43e0(long param_1,long **param_2);
void ifio_d628d.e8181d6173042c8e018f5187cf2913d540c.3.2.16.1731(long **param_1);
undefined8 ifio_f3158.2affa03c04c8dcc486cbea33f1f89cce608.3.2.16.1731(long param_1,short **param_2,long *param_3,long *param_4);
undefined8 ifio_0719f.525115d2fd82d18d81ad55b976fba96a858.3.2.16.1731(long param_1,long **param_2,long **param_3);
undefined8 ifio_caa09.b678f1c0e20f668e050a78efbf29bc3077e.3.2.16.1731(long param_1,short **param_2,long *param_3,long *param_4);
void FUN_001d4dc0(long **param_1);
undefined8 FUN_001d4e10(long param_1,long **param_2);
void ifio_e9753.f9fd9973b79c346cdad9a66df6ec3b1a500.3.2.16.1731(long **param_1);
undefined8 ifio_7d269.9815f003cc82dceecc7c997667573b03cc7.3.2.16.1731(int *param_1,long **param_2,long *param_3,long *param_4);
undefined8 ifio_19af2.37988b1f0f8752e8eb200d8ba6e62101dde.3.2.16.1731(int *param_1,long **param_2,long **param_3);
undefined8 ifio_cac0a.ee5abd1d0e92ca8281acdf89bc1f6823074.3.2.16.1731(long param_1,undefined8 param_2,long *param_3,long *param_4);
undefined8 ifio_d1f68.51c9c7d4979148af4ffca718fe8f866bb33.3.2.16.1731(long param_1,long **param_2,long **param_3);
void ifio_c857b.e1db15734bf8eb950e6a96a7b46579bf98a.3.2.16.1731(long **param_1);
undefined8 ifio_16705.b5ea2dbf1411a6c1d1a95390f9c0b6ea008.3.2.16.1731(long param_1,long **param_2,long **param_3);
undefined8 ifio_c53ba.1a14c43e1bab380be44e5178a6457fa75b8.3.2.16.1731(long param_1,undefined8 param_2,undefined8 *param_3,long *param_4);
void ifio_34f2a.2eceda1e2b89a48c8cda7e4331440e76788.3.2.16.1731(long **param_1);
undefined4 FUN_001d5590(int *param_1,long param_2,ushort *param_3,long param_4,int param_5,int param_6,uint *param_7,uint *param_8);
void ifio_e313b.afd5c3b939139c57707a7164d467d6f56c0.3.2.16.1731(undefined *param_1,undefined8 param_2,uint param_3,uint param_4,int param_5);
void ifio_8a4b2.17fc4ec76d43a03b70e323a3eb927e1c20d.3.2.16.1731(char *param_1,undefined8 param_2);
void ifio_6f7da.d7bfc7ce119a7aae1ebdd26108e21dd5d57.3.2.16.1731(void);
void ifio_e256f.580b6faebbe01156e1d731c187953db4d06.3.2.16.1731(void);
void ifio_69844.add518a910c805ef38febfc481e9501bbdb.3.2.16.1731(int *param_1,ushort *param_2,char *param_3,int param_4,int param_5,undefined8 param_6,int *param_7,int param_8);
void ifio_2dd8a.846b86f69b74bc561581ba5130886cd44f9.3.2.16.1731(int *param_1,long param_2,ushort *param_3,undefined8 param_4,long param_5,int param_6,int param_7);
void ifio_7db2e.45b09e8f42cbbfc7a1cc6ee70c870cb983e.3.2.16.1731(int *param_1,long param_2,undefined8 param_3,long param_4,int param_5,uint *param_6,uint *param_7);
void ifio_0b0e9.73f5347a304cb5848ed589949c882c0ae22.3.2.16.1731(int param_1,int param_2,uint param_3,ushort param_4);
undefined8 ifio_abf48.4f924e4a70a3ae5ee195c40ddad5005fa10.3.2.16.1731(byte *param_1,byte *param_2);
void ifio_58067.5419e8a243d20ff5fc441394fa1a1cd8a84.3.2.16.1731(undefined *param_1,undefined *param_2);
void ifio_bfcb5.5f543e933d58cfa491fb49371cac7ad8782.3.2.16.1731(ushort *param_1,uint param_2,int param_3);
void ifio_21326.1b60ef450dbc2a70282033b3600fc5f78d1.3.2.16.1731(long param_1);
void ifio_7f832.cf534fded9b692d6703087ca20a89e91675.3.2.16.1731(undefined *param_1,undefined8 param_2,undefined8 param_3,undefined8 param_4,undefined8 param_5);
void ifio_9a491.e5a3ae26d470a7b86c1d8985cc069171ad7.3.2.16.1731(long param_1);
void ifio_d7f8c.fb0814601d5d6fc3e1bf188290e1e28ef7a.3.2.16.1731(long param_1);
uint ifio_7c150.7c31d6794f16ce4d8416858ffe1fd9f40eb.3.2.16.1731(long param_1,char *param_2,byte param_3);
void ifio_a0688.ef24c46fa985a292b5eb537144be121b3b7.3.2.16.1731(long param_1);
undefined8 ifio_8d635.3433f6d5fe890c0dd0f6203dffbf8eacb0c.3.2.16.1731(long param_1);
undefined4 ifio_82f0d.1ac2c7c93c0a7dedbd3076e2f13ac310cdc.3.2.16.1731(long param_1,char *param_2,char *param_3,undefined8 param_4,undefined8 param_5,byte param_6);
void ifio_2cd76.05cd67c550e4515c4e2843f203ef5eb0b9b.3.2.16.1731(long param_1,byte **param_2);
undefined4 ifio_4366e.c5d582f345f651aa24d997f0a63b6d1bfcb.3.2.16.1731(byte *param_1,byte *param_2,undefined8 param_3,undefined8 param_4,byte param_5);
undefined4 ifio_a271c.5b544736ef62a605d217c778eec56e0bb36.3.2.16.1731(long param_1,byte *param_2,undefined8 param_3,undefined8 param_4,byte param_5);
void ifio_d380e.becad63929ab36af4f9d0cad91009a3a02b.3.2.16.1731(undefined *param_1,undefined8 param_2);
void ifio_20047.1e35b7525bcc6f3a36134619bbf53d1eff5.3.2.16.1731(long param_1,undefined8 param_2);
undefined8 ifio_f2d70.8db42da0d9d3077105b2565b6aee727eb38.3.2.16.1731(long param_1,undefined param_2,undefined *param_3);
void ifio_aacc8.511afefa80dc15850a3dc5a25a5ae5122fb.3.2.16.1731(undefined *param_1,undefined *param_2);
char * ifio_2ac44.3796385ead692878ec14ddb3d4098391cf1.3.2.16.1731(char *param_1);
void FUN_001d6e80(long param_1,long param_2);
void ifio_3a4b3.0b47a0bcc3363a61c5638062813eb517bee.3.2.16.1731(long param_1);
undefined8 ifio_2db0d.e46c9f15e909bc238de127a41c9e2ef27e9.3.2.16.1731(undefined *param_1,undefined param_2,undefined8 param_3,undefined8 param_4);
void ifio_f77ec.cb8af4f64bc3c999b953d3a300650c7fa14.3.2.16.1731(long param_1,char *param_2,long param_3);
void ifio_2e75f.945b8bbb0713966c637f584cf2b0bb9bbc6.3.2.16.1731(undefined *param_1);
void ifio_bb91e.706bf902d89ef7ab6f82f9306f4f490095f.3.2.16.1731(long param_1,long param_2,byte *param_3);
undefined8 ifio_2e5a0.118b032d6e4a065b2493bc862a3730e0406.3.2.16.1731(undefined *param_1,undefined param_2,undefined8 param_3,undefined8 param_4);
undefined8 ifio_cc5fc.22f061b27f699ff2bf3ab9efbae5ab90825.3.2.16.1731(undefined *param_1,undefined *param_2);
void ifio_b56c6.1360f28949e5f99f810d88bb6f8b753e0a9.3.2.16.1731(long param_1,undefined param_2,undefined param_3,undefined param_4);
void ifio_fb5a3.f6326baee4c31487f9d44b7deff30a1226c.3.2.16.1731(long param_1,byte *param_2,undefined param_3,undefined param_4,undefined param_5,undefined4 param_6);
void ifio_90188.a80bdda093015a2eebc8622b421ad2b9bef.3.2.16.1731(undefined *param_1,long param_2,long param_3);
void ifio_abe11.d7144e402f448a36d51ba9951df44a5bbcb.3.2.16.1731(long param_1,long param_2,long param_3);
void FUN_001d7d18(ushort param_1,long param_2,undefined8 param_3,undefined8 param_4,undefined8 param_5,undefined8 param_6,undefined param_7);
undefined  [16] FUN_001d7d77(long param_1,ushort param_2,undefined8 param_3,undefined8 param_4);
void FUN_001d7de6(long param_1,uint param_2,uint param_3,int param_4,uint param_5,ulong param_6,int param_7,uint param_8,uint param_9,int param_10);
undefined  [16] FUN_001d8228(long param_1,long param_2,undefined8 param_3,undefined8 param_4);
undefined kfio_dma_sync()
undefined fusion_rwsem_init()
undefined kfio_cache_free()
undefined kfio_pci_get_node()
undefined fusion_condvar_signal()
undefined __kfio_create_cache()
undefined kfio_sgl_alloc_nvec()
undefined fusion_mutex_lock()
undefined kfio_disk_stat_write_update()
undefined kfio_pci_enable_device()
undefined fusion_usectohz()
undefined fusion_spin_lock_irqsave_nested()
undefined fusion_rwsem_down_read()
undefined __fio_wait_on_states()
undefined kfio_get_user_pages()
void * memmove(void * __dest, void * __src, size_t __n)
undefined kfio_sgl_dma_map()
undefined kfio_pci_disable_msix()
undefined kfio_sgl_dma_slice()
undefined kfio_pci_name()
undefined fio_transition_state()
undefined kfio_iounmap()
undefined kfio_pci_write_config_word()
undefined kfio_poll_wake()
undefined kfio_print()
undefined kfio_sgl_dma_unmap()
undefined fusion_cond_resched()
undefined kfio_mark_lock_pending()
undefined fio_state_down()
undefined kfio_cpu_online()
undefined kfio_put_cpu()
undefined fusion_destroy_control_device()
undefined kfio_current_cpu()
undefined fusion_destroy_spin()
undefined fusion_mutex_init()
undefined kfio_pci_disable_device()
undefined kfio_csr_read_direct_64()
undefined kfio_sgl_copy_data()
undefined kfio_pci_get_subsystem_vendor()
undefined fusion_spin_unlock()
undefined kfio_pci_write_config_dword()
undefined fusion_condvar_destroy()
undefined kfio_msleep_noload()
undefined kfio_register_blkdev_pre_init()
undefined kfio_unregister_cpu_notifier()
undefined kfio_pci_bus_self()
undefined kfio_vfree()
undefined kfio_dma_alloc_coherent()
undefined fusion_HZ()
undefined kfio_pci_enable_msi()
undefined fusion_condvar_broadcast()
undefined kfio_strncmp()
undefined kfio_pci_read_config_word()
undefined fusion_init_spin()
undefined fusion_init_work()
undefined fusion_rwsem_delete()
undefined kfio_sgl_map_bytes()
undefined fusion_get_lbolt()
undefined kfio_unmark_lock_pending()
undefined kfio_pci_read_config_dword()
undefined kfio_pci_get_domain()
undefined kfio_malloc_node()
undefined kfio_pci_get_vendor()
undefined kfio_dma_map_free()
undefined kfio_free_irq()
undefined fusion_condvar_timedwait_noload()
undefined ifio_strerror_sym()
undefined kfio_dma_free_coherent()
undefined fusion_destroy_work()
undefined kfio_pci_get_function()
undefined kfio_get_cpu()
undefined fio_state_not_in_one_of()
undefined fusion_hztousec()
undefined fusion_rwsem_up_write()
undefined fusion_rwsem_down_read_trylock()
undefined kfio_pci_set_dma_mask()
undefined kfio_next_cpu_in_node()
undefined kfio_pci_get_bus()
undefined kfio_poll_init()
undefined kfio_get_msix_number()
undefined fusion_getwallclocktime()
undefined kfio_get_seconds()
undefined kfio_fetch_next_bio()
undefined kfio_sgl_map_page()
undefined fio_state_transition()
undefined kfio_destroy_disk()
undefined kfio_dma_map_nvecs()
undefined kfio_pci_bus_parent()
undefined kfio_put_user_pages()
undefined fusion_create_kthread()
undefined kfio_memcpy()
undefined kfio_ioremap_nocache()
undefined kfio_pci_set_master()
undefined kfio_vmalloc()
undefined kfio_bind_kthread_to_node()
undefined kfio_msleep()
undefined kfio_request_msix()
undefined kfio_sgl_destroy()
undefined fusion_rwsem_up_read()
undefined kfio_csr_read_direct()
undefined fusion_schedule_work()
undefined kfio_pci_get_devnum()
undefined kfio_sgl_map_user_pages()
undefined fusion_spin_lock()
undefined kfio_strcmp()
undefined kfio_alloc_0_page()
undefined kfio_info_os_create_node()
undefined kfio_request_irq()
void * memset(void * __s, int __c, size_t __n)
undefined kfio_udelay()
undefined fusion_spin_unlock_irqrestore()
undefined fusion_spin_trylock()
undefined kfio_strtoul()
undefined kfio_memmove()
undefined fusion_schedule_yield()
undefined fusion_condvar_init()
undefined kfio_cache_alloc_node()
undefined kfio_malloc_atomic()
undefined kfio_iodrive_intx()
undefined fusion_condvar_timedwait()
undefined kfio_free()
undefined fusion_create_control_device()
undefined kfio_memcmp()
undefined kfio_pci_request_regions()
undefined fusion_spin_lock_irqdisabled()
undefined kfio_pci_release_regions()
undefined kfio_csr_write_64()
undefined fusion_getmicrotime()
undefined fio_state_up()
undefined kfio_strncpy()
undefined kfio_pci_set_drvdata()
undefined kfio_free_msix()
undefined kfio_dma_map_alloc()
undefined kfio_pci_get_subsystem_device()
undefined kfio_pci_resource_start()
undefined kfio_create_kthread_on_cpu()
undefined fusion_mutex_destroy()
undefined kfio_snprintf()
undefined kfio_create_disk()
undefined kfio_register_cpu_notifier()
undefined kfio_get_irq_number()
undefined kfio_pci_read_config_byte()
undefined kfio_dump_stack()
undefined kfio_cache_destroy()
undefined kfio_strlen()
undefined kfio_copy_to_user()
undefined kfio_bus_from_pci_dev()
undefined fio_transition_state_va()
undefined kfio_sgl_size()
undefined kfio_cache_alloc()
undefined kfio_csr_write()
undefined fusion_might_sleep()
undefined fusion_spin_lock_irqsave()
undefined fusion_mutex_unlock()
undefined kfio_vsnprintf()
undefined kfio_free_page()
undefined kfio_max_cpus()
undefined fusion_rwsem_down_write()
undefined kfio_get_numa_node_override()
undefined kfio_pci_enable_msix()
undefined kfio_sgl_reset()
undefined fusion_mutex_trylock()
undefined kfio_info_os_remove_node()
undefined kfio_expose_disk()
undefined kfio_page_address()
undefined kfio_dma_map_next()
undefined kfio_dma_map_first()
undefined kfio_pci_disable_msi()
undefined kfio_set_gd_in_flight()
undefined kfio_pci_write_config_byte()
undefined kfio_memset()
undefined kfio_copy_from_user()
undefined ifio_strerror()
undefined fusion_condvar_wait()
undefined kfio_pci_get_devicenum()
undefined kfio_page_from_virt()
undefined kfio_malloc()
undefined fio_state_in_one_of()
undefined kfio_pci_get_drvdata()
undefined kfio_strncat()
undefined kfio_disk_stat_read_update()
undefined fusion_rwsem_down_write_trylock()
undefined kfio_pci_resource_len()
undefined kfio_sgl_dump()
undefined kfio_pci_get_slot()
undefined kfio_pci_bus_istop()

