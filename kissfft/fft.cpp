#include "fft.h"
#include <vector>
#include "kiss_fftr.h"

/**
 * @brief findXCorrMax_fft
 *
 * Returns the lag at which the cross-correlation between signals
 * 'a' and 'b' is at its maximum. When 'a' lags behind 'b', the
 * result is positive. When 'b' lags behind 'a', the result is
 * negative.
 *
 * @param aData The samples (as int16 array) for signal a.
 * @param aCnt The number of samples in signal a.
 * @param bData The samples (as int16 array) for signal b.
 * @param bCnt The number of samples in signal b.
 * @param peakWidth The number of samples that should be ignored
 * around the peak when determining the score.
 * @param score Output argument. Holds a score (between 0 and 1)
 * indicating the uniqueness of the match.
 * @return The index at which the maximum correlation was
 * found.
 */
int xcorrMax(int16_t* aData, size_t aCnt, int16_t* bData, size_t bCnt, int peakWidth, float *score) {
    // find number of fft point to use (s.t. both signals are max for 50% filled)
    int nfft = (int) std::max(aCnt, bCnt) * 2;
    nfft = kiss_fft_next_fast_size(nfft); // make it a nice and fast fft size
    int n;
    
    kiss_fftr_cfg fwdCfg = kiss_fftr_alloc(nfft, 0, NULL, NULL); // forward fft configuration
    kiss_fftr_cfg invCfg = kiss_fftr_alloc(nfft, 1, NULL, NULL); // inverse fft configuration
    
    // real fft for signal a
    std::vector<kiss_fft_scalar> aTim(nfft, 0);   // create float time domain container (zero-padded)
    std::copy(aData, aData + aCnt, aTim.begin()); // copy data and convert int16 to float
    std::vector<kiss_fft_cpx> aFrq(nfft / 2 + 1); // create complex freq domain container
    kiss_fftr(fwdCfg, aTim.data(), aFrq.data());  // perform fft
    
    // real fft for signal b
    std::vector<kiss_fft_scalar> bTim(nfft, 0);   // create float time domain container (zero-padded)
    std::copy(bData, bData + bCnt, bTim.begin()); // copy data and convert int16 to float
    std::vector<kiss_fft_cpx> bFrq(nfft / 2 + 1); // create complex freq domain container
    kiss_fftr(fwdCfg, bTim.data(), bFrq.data());  // perform fft
    
    // multiply a with the conjugate of b in the freq domain (store in a)
    for (n = 0; n < nfft / 2 + 1; n++) {
        kiss_fft_scalar r = aFrq[n].r * bFrq[n].r + aFrq[n].i * bFrq[n].i;
        kiss_fft_scalar i = aFrq[n].i * bFrq[n].r - aFrq[n].r * bFrq[n].i;
        aFrq[n].r = r;
        aFrq[n].i = i;
    }
    
    // real inverse fft of the multiplied freq domain (store in a)
    kiss_fftri(invCfg, aFrq.data(), aTim.data());
    
    // find the maximum value in the resulting time domain signal
    kiss_fft_scalar maxValue = 0;
    int maxOffset = 0;
    for (n = 0; n < nfft; n++) {
        if (fabsf(aTim[n]) > maxValue) {
            maxValue = fabsf(aTim[n]);
            maxOffset = n;
        }
    }
    
    // find the second highest value in the time domain signal that is at
    // least peakWidth samples away from the highest value
    kiss_fft_scalar secondValue = 0;
    for (n = 0; n < nfft; n++) {
        int diff = n - maxOffset;
        if (diff < -nfft / 2) diff += nfft;
        if ((abs(diff) > peakWidth) && (fabsf(aTim[n]) > secondValue)) {
            secondValue = fabsf(aTim[n]);
        }
    }
    
    // unwrap the resulting offset
    if (maxOffset > nfft / 2) maxOffset -= nfft;
    
    // output values
    if (score) *score = (secondValue / maxValue);
    return maxOffset;
}
