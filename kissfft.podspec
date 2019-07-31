Pod::Spec.new do |s|
  s.name = "kissfft"
  s.version = "1.3.0"
  s.summary = "KISS FFT - A mixed-radix Fast Fourier Transform based up on the principle, 'Keep It Simple, Stupid.'"
  s.homepage = "https://sourceforge.net/projects/kissfft/"
  s.license = { :type => "BSD", :file => "LICENSE" }
  s.author = "Mark Borgerding"
  s.ios.deployment_target = "10.0"
  s.osx.deployment_target = "10.7"
  s.source = { :git => 'git@git.filmicpro.com:iOS/memory-mic/kissfft.git', :tag => s.version }
  s.source_files  = "kissfft/*.{h,c,cpp}"
  s.public_header_files = "kissfft/fft.h"
  s.private_header_files = [
    "kissfft/_kiss_fft_guts.h",
    "kissfft/kiss_fft.h",
    "kissfft/kiss_fftr.h",
  ]
end
