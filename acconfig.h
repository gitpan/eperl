#ifndef AC_H
#define AC_H
/*
**
**  ac.h -- AUTO configuration header file
**  Copyright (c) Ralf S. Engelschall, <rse@en.muc.de>
**
*/

@TOP@


/* Define if system supports prototypes, i.e. the compiler
   may be a real ANSI compiler */
#undef HAVE_PROTOTYPES

/* Define if g++ is of old brainded version 2.5.8 which
   doesn't support __FUNCTION__ while its gcc does */
#undef HAVE_GXX_NO_FUNCTION_DEFINE

@BOTTOM@

#endif /* AC_H */
