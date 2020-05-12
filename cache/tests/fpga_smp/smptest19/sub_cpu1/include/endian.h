#if !defined (_UCENDIAN_H_)
#define _UCENDIAN_H_


#define bswap_16(x) \
    (__extension__                                                            \
     ({ unsigned short int __bsx = (x);                                       \
        ((((__bsx) >> 8) & 0xff) | (((__bsx) & 0xff) << 8)); }))

#define bswap_32(x) \
    (__extension__                                                            \
     ({ unsigned int __bsx = (x);                                             \
        ((((__bsx) & 0xff000000) >> 24) | (((__bsx) & 0x00ff0000) >>  8) |    \
         (((__bsx) & 0x0000ff00) <<  8) | (((__bsx) & 0x000000ff) << 24)); }))

#define	__LITTLE_ENDIAN	1234
#define	__BIG_ENDIAN	4321


#if defined(CONFIG_BIG_ENDIAN)
#  define	__BYTE_ORDER	__BIG_ENDIAN

  /* note "ltoh" = little-endian to host */
#  define ltoh16(x)  bswap_16(x)
#  define ltoh32(x)  bswap_32(x)

#elif defined(CONFIG_LITTLE_ENDIAN)
#  define	__BYTE_ORDER	__LITTLE_ENDIAN

  /* note "ltoh" = little-endian to host */
#  define ltoh16(x)  (x)
#  define ltoh32(x)  (x)

#endif


#endif /* defined _UCENDIAN_H_ */
