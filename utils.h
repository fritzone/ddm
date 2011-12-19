#ifndef UTILS_H
#define UTILS_H

template <class T> const T& max ( const T& a, const T& b )
{
  return (b<a)?a:b;
}

#endif // UTILS_H
