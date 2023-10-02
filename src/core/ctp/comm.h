#ifndef B43732C7_EA9D_4138_8023_E0627CD66A48
#define B43732C7_EA9D_4138_8023_E0627CD66A48

#define CTP_COPY_SAFE( _field_, _str_ ) memcpy( _filed_, _str_, std::min( strlen( _str_ ), sizeof( _field_ ) - 1 ) )

#endif /* B43732C7_EA9D_4138_8023_E0627CD66A48 */
