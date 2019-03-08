VC: A C++ LIBRARY FOR EXPLICIT VECTORIZATION 11
typedef std::complex<float> Z;
for (int y = 0; y < imageHeight; ++y) {
const float c_imag = y0 + y * scale; for (int x = 0; x < imageWidth; ++x) {
Z c(x0 + x * scale, c_imag);
Z z = c;
int n = 0;
for (; n < maxIt && fastNorm(z) < S; ++n) {
z = z * z + c; }
const uchar colorValue = (maxIt - n) * 255 / maxIt;
    colorizeNextPixel(colorValue);
  }
}
Figure 7. Scalar implementation of the Mandelbrot iteration
typedef std::complex<float_v> Z;
for (int y = 0; y < imageHeight; ++y) {
const float_v c_imag = y0 + y * scale; for (int_v x = int_v::IndexesFromZero();
       !(x < imageWidth).isEmpty();
x += float_v::Size) {
Z c(x0 + static_cast<float_v>(x) * scale, c_imag); Z z = c;
int_v n = int_v::Zero();
int_m inside = fastNorm(z) < S;
while (!(inside && n < maxIt).isEmpty()) {
      z = z * z + c;
      ++n(inside);
      inside = fastNorm(z) < S;
}
const int_v colorValue = (maxIt - n) * 255 / maxIt; int maxJ = min(int_v::Size, imageWidth - x[0]);
for (int j = 0; j < maxJ; ++j) {
      colorizeNextPixel(colorValue[j]);
    }
} }
