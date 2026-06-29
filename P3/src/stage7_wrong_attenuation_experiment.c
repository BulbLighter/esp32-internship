// PROJECT 3 — Stage 7A: Wrong Attenuation Experiment
// Deliberately using default 0dB attenuation (0-1.1V range only)

const int LDR_PIN = 35;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // DELIBERATELY NOT setting attenuation — using default ADC_0db
  // analogSetAttenuation(ADC_11db);  <-- commented out on purpose
  
  Serial.println("=== Stage 7A: WRONG Attenuation (0dB, 0-1.1V range only) ===");
  Serial.println("Expect saturation at 4095 for anything above ~1.1V");
  Serial.println();
}

void loop() {
  int raw = analogRead(LDR_PIN);
  float voltage = (raw / 4095.0) * 3.3;  // This calculation will now be WRONG
  
  Serial.print("ADC Raw: ");
  Serial.print(raw);
  Serial.print(" (calculated as ");
  Serial.print(voltage, 2);
  Serial.println("V — but this is INCORRECT due to wrong attenuation)");
  
  delay(500);
}
// experiment 1
//dim light
//
// ADC Raw: 238 (calculated as 0.19V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 272 (calculated as 0.22V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 247 (calculated as 0.20V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 281 (calculated as 0.23V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 279 (calculated as 0.22V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 303 (calculated as 0.24V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 306 (calculated as 0.25V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 305 (calculated as 0.25V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 307 (calculated as 0.25V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 315 (calculated as 0.25V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 294 (calculated as 0.24V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 325 (calculated as 0.26V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 289 (calculated as 0.23V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 285 (calculated as 0.23V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 271 (calculated as 0.22V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 281 (calculated as 0.23V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 252 (calculated as 0.20V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 244 (calculated as 0.20V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 240 (calculated as 0.19V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 255 (calculated as 0.21V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 240 (calculated as 0.19V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 261 (calculated as 0.21V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 256 (calculated as 0.21V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 241 (calculated as 0.19V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 295 (calculated as 0.24V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 263 (calculated as 0.21V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 282 (calculated as 0.23V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 297 (calculated as 0.24V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 307 (calculated as 0.25V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 294 (calculated as 0.24V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 310 (calculated as 0.25V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 315 (calculated as 0.25V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 304 (calculated as 0.24V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 310 (calculated as 0.25V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 294 (calculated as 0.24V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 289 (calculated as 0.23V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 291 (calculated as 0.23V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 278 (calculated as 0.22V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 270 (calculated as 0.22V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 258 (calculated as 0.21V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 241 (calculated as 0.19V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 231 (calculated as 0.19V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 247 (calculated as 0.20V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 210 (calculated as 0.17V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 235 (calculated as 0.19V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 288 (calculated as 0.23V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 260 (calculated as 0.21V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 257 (calculated as 0.21V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 259 (calculated as 0.21V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 277 (calculated as 0.22V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 279 (calculated as 0.22V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 277 (calculated as 0.22V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 303 (calculated as 0.24V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 304 (calculated as 0.24V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 311 (calculated as 0.25V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 319 (calculated as 0.26V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 317 (calculated as 0.26V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 311 (calculated as 0.25V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 267 (calculated as 0.22V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 273 (calculated as 0.22V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 311 (calculated as 0.25V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 294 (calculated as 0.24V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 283 (calculated as 0.23V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 277 (calculated as 0.22V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 274 (calculated as 0.22V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 304 (calculated as 0.24V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 248 (calculated as 0.20V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 247 (calculated as 0.20V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 241 (calculated as 0.19V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 252 (calculated as 0.20V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 244 (calculated as 0.20V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 272 (calculated as 0.22V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 247 (calculated as 0.20V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 240 (calculated as 0.19V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 259 (calculated as 0.21V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 268 (calculated as 0.22V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 277 (calculated as 0.22V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 290 (calculated as 0.23V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 288 (calculated as 0.23V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 304 (calculated as 0.24V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 302 (calculated as 0.24V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 289 (calculated as 0.23V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 317 (calculated as 0.26V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 304 (calculated as 0.24V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 288 (calculated as 0.23V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 305 (calculated as 0.25V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 304 (calculated as 0.24V — but this is INCORRECT due to wrong attenuation)

// phone flashlight

// ADC Raw: 1911 (calculated as 1.54V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1967 (calculated as 1.59V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1919 (calculated as 1.55V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1922 (calculated as 1.55V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1931 (calculated as 1.56V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1927 (calculated as 1.55V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1931 (calculated as 1.56V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1911 (calculated as 1.54V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1930 (calculated as 1.56V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1905 (calculated as 1.54V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1921 (calculated as 1.55V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1914 (calculated as 1.54V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1846 (calculated as 1.49V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1910 (calculated as 1.54V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1904 (calculated as 1.53V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1910 (calculated as 1.54V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1906 (calculated as 1.54V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1903 (calculated as 1.53V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1898 (calculated as 1.53V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1910 (calculated as 1.54V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1902 (calculated as 1.53V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1900 (calculated as 1.53V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1894 (calculated as 1.53V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1890 (calculated as 1.52V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1901 (calculated as 1.53V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1904 (calculated as 1.53V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1913 (calculated as 1.54V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1901 (calculated as 1.53V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1904 (calculated as 1.53V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1904 (calculated as 1.53V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1877 (calculated as 1.51V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1903 (calculated as 1.53V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1900 (calculated as 1.53V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1872 (calculated as 1.51V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1901 (calculated as 1.53V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1899 (calculated as 1.53V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1900 (calculated as 1.53V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1904 (calculated as 1.53V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1891 (calculated as 1.52V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1904 (calculated as 1.53V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1911 (calculated as 1.54V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1913 (calculated as 1.54V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1904 (calculated as 1.53V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1907 (calculated as 1.54V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1917 (calculated as 1.54V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1919 (calculated as 1.55V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1904 (calculated as 1.53V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1919 (calculated as 1.55V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1922 (calculated as 1.55V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1851 (calculated as 1.49V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1894 (calculated as 1.53V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1902 (calculated as 1.53V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1919 (calculated as 1.55V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1918 (calculated as 1.55V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1943 (calculated as 1.57V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1922 (calculated as 1.55V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1920 (calculated as 1.55V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1915 (calculated as 1.54V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1916 (calculated as 1.54V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1907 (calculated as 1.54V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1915 (calculated as 1.54V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1917 (calculated as 1.54V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1920 (calculated as 1.55V — but this is INCORRECT due to wrong attenuation)
// ADC Raw: 1901 (calculated as 1.53V — but this is INCORRECT due to wrong attenuation)

// experiment 2
//Read 1: 1755
// Read 2: 1755
// Read 3: 1745
// Read 4: 1729
// Read 5: 1759
// Read 6: 1744
// Read 7: 1749
// Read 8: 1745
// Read 9: 1751
// Read 10: 1719
// Read 11: 1771
// Read 12: 1748
// Read 13: 1750
// Read 14: 1775
// Read 15: 1744
// Read 16: 1744
// Read 17: 1759
// Read 18: 1749
// Read 19: 1744
// Read 20: 1747
// Read 21: 1792
// Read 22: 1747
// Read 23: 1755
// Read 24: 1741
// Read 25: 1749
// Read 26: 1749
// Read 27: 1746
// Read 28: 1743
// Read 29: 1761
// Read 30: 1747
// Read 31: 1742
// Read 32: 1738
// Read 33: 1751
// Read 34: 1746
// Read 35: 1746
// Read 36: 1756
// Read 37: 1759
// Read 38: 1741
// Read 39: 1749
// Read 40: 1755
// Read 41: 1745
// Read 42: 1744
// Read 43: 1746
// Read 44: 1749
// Read 45: 1712
// Read 46: 1744
// Read 47: 1746
// Read 48: 1744
// Read 49: 1749
// Read 50: 1776
// Read 51: 1747
// Read 52: 1743
// Read 53: 1739
// Read 54: 1745
// Read 55: 1728
// Read 56: 1737
// Read 57: 1747
// Read 58: 1749
// Read 59: 1744
// Read 60: 1744
// Read 61: 1744
// Read 62: 1658
// Read 63: 1738
// Read 64: 1739
// Read 65: 1743
// Read 66: 1742
// Read 67: 1799
// Read 68: 1744
// Read 69: 1755
// Read 70: 1744
// Read 71: 1744
// Read 72: 1738
// Read 73: 1751
// Read 74: 1751
// Read 75: 1743
// Read 76: 1743
// Read 77: 1776
// Read 78: 1751
// Read 79: 1751
// Read 80: 1789
// Read 81: 1741
// Read 82: 1733
// Read 83: 1747
// Read 84: 1758
// Read 85: 1745
// Read 86: 1756
// Read 87: 1746
// Read 88: 1738
// Read 89: 1761
// Read 90: 1745
// Read 91: 1750
// Read 92: 1745
// Read 93: 1749
// Read 94: 1749
// Read 95: 1746
// Read 96: 1748
// Read 97: 1791
// Read 98: 1744
// Read 99: 1746
// Read 100: 1730

// === 100 reads complete. Pausing 5 seconds... ===

// Read 1: 1751
// Read 2: 1840
// Read 3: 1735
// Read 4: 1776
// Read 5: 1746
// Read 6: 1744
// Read 7: 1745
// Read 8: 1753
// Read 9: 1743
// Read 10: 1748
// Read 11: 1747
// Read 12: 1751
// Read 13: 1758
// Read 14: 1744
// Read 15: 1765
// Read 16: 1732
// Read 17: 1746
// Read 18: 1744
// Read 19: 1741
// Read 20: 1744
// Read 21: 1744
// Read 22: 1799
// Read 23: 1739
// Read 24: 1731
// Read 25: 1747
// Read 26: 1745
// Read 27: 1744
// Read 28: 1745
// Read 29: 1746
// Read 30: 1752
// Read 31: 1757
// Read 32: 1751
// Read 33: 1744
// Read 34: 1745
// Read 35: 1748
// Read 36: 1745
// Read 37: 1744
// Read 38: 1746
// Read 39: 1747
// Read 40: 1745
// Read 41: 1745
// Read 42: 1749
// Read 43: 1745
// Read 44: 1731
// Read 45: 1744
// Read 46: 1744
// Read 47: 1746
// Read 48: 1742
// Read 49: 1754
// Read 50: 1744
// Read 51: 1747
// Read 52: 1744
// Read 53: 1749
// Read 54: 1743
// Read 55: 1744
// Read 56: 1754
// Read 57: 1743
// Read 58: 1722
// Read 59: 1762
// Read 60: 1744
// Read 61: 1745
// Read 62: 1756
// Read 63: 1739
// Read 64: 1733
// Read 65: 1766
// Read 66: 1752
// Read 67: 1765
// Read 68: 1761
// Read 69: 1683
// Read 70: 1746
// Read 71: 1751
// Read 72: 1728
// Read 73: 1744
// Read 74: 1746
// Read 75: 1734
// Read 76: 1749
// Read 77: 1739
// Read 78: 1744
// Read 79: 1744
// Read 80: 1744
// Read 81: 1756
// Read 82: 1774
// Read 83: 1732
// Read 84: 1771
// Read 85: 1746
// Read 86: 1750
// Read 87: 1745
// Read 88: 1746
// Read 89: 1760
// Read 90: 1738
// Read 91: 1747
// Read 92: 1750
// Read 93: 1749
// Read 94: 1742
// Read 95: 1739
// Read 96: 1745
// Read 97: 1742
// Read 98: 1739
// Read 99: 1743
// Read 100: 1743

// === 100 reads complete. Pausing 5 seconds... ===

// Read 1: 1751
// Read 2: 1758
// Read 3: 1744
// Read 4: 1745
// Read 5: 1717
// Read 6: 1747
// Read 7: 1744
// Read 8: 1744
// Read 9: 1744
// Read 10: 1775
// Read 11: 1751
// Read 12: 1748
// Read 13: 1751
// Read 14: 1745
// Read 15: 1744
// Read 16: 1741
// Read 17: 1745
// Read 18: 1761
// Read 19: 1725
// Read 20: 1751
// Read 21: 1741
// Read 22: 1747
// Read 23: 1724
// Read 24: 1757
// Read 25: 1745
// Read 26: 1737
// Read 27: 1650
// Read 28: 1653
// Read 29: 1747
// Read 30: 1747
// Read 31: 1731
// Read 32: 1750
// Read 33: 1745
// Read 34: 1751
// Read 35: 1742
// Read 36: 1744
// Read 37: 1747
// Read 38: 1759
// Read 39: 1715
// Read 40: 1737
// Read 41: 1743
// Read 42: 1750
// Read 43: 1728
// Read 44: 1742
// Read 45: 1773
// Read 46: 1731
// Read 47: 1749
// Read 48: 1745
// Read 49: 1751
// Read 50: 1750
// Read 51: 1745
// Read 52: 1747
// Read 53: 1756
// Read 54: 1743
// Read 55: 1745
// Read 56: 1745
// Read 57: 1729
// Read 58: 1746
// Read 59: 1744
// Read 60: 1759
// Read 61: 1741
// Read 62: 1746
// Read 63: 1743
// Read 64: 1744
// Read 65: 1744
// Read 66: 1747
// Read 67: 1743
// Read 68: 1775
// Read 69: 1759
// Read 70: 1749
// Read 71: 1751
// Read 72: 1747
// Read 73: 1744
// Read 74: 1744
// Read 75: 1750
// Read 76: 1734
// Read 77: 1743
// Read 78: 1744
// Read 79: 1744
// Read 80: 1744
// Read 81: 1739
// Read 82: 1744
// Read 83: 1741
// Read 84: 1763
// Read 85: 1753
// Read 86: 1747
// Read 87: 1744
// Read 88: 1744
// Read 89: 1749
// Read 90: 1746
// Read 91: 1754
// Read 92: 1744
// Read 93: 1735
// Read 94: 1742
// Read 95: 1744
// Read 96: 1751
// Read 97: 1745
// Read 98: 1750
// Read 99: 1738
// Read 100: 1743

// === 100 reads complete. Pausing 5 seconds... ===

// Read 1: 1747
// Read 2: 1756
// Read 3: 1767
// Read 4: 1728
// Read 5: 1753
// Read 6: 1746
// Read 7: 1744
// Read 8: 1744
// Read 9: 1744
// Read 10: 1744
// Read 11: 1722
// Read 12: 1751
// Read 13: 1765
// Read 14: 1757
// Read 15: 1712
// Read 16: 1742
// Read 17: 1771
// Read 18: 1750
// Read 19: 1743
// Read 20: 1744
// Read 21: 1751
// Read 22: 1731
// Read 23: 1744
// Read 24: 1741
// Read 25: 1750
// Read 26: 1749
// Read 27: 1735
// Read 28: 1759
// Read 29: 1744
// Read 30: 1757
// Read 31: 1762
// Read 32: 1745
// Read 33: 1747
// Read 34: 1744
// Read 35: 1744
// Read 36: 1751
// Read 37: 1755
// Read 38: 1744
// Read 39: 1746
// Read 40: 1744
// Read 41: 1744
// Read 42: 1761
// Read 43: 1744
// Read 44: 1786
// Read 45: 1745
// Read 46: 1770
// Read 47: 1748
// Read 48: 1749
// Read 49: 1712
// Read 50: 1750
// Read 51: 1759
// Read 52: 1744
// Read 53: 1745
// Read 54: 1744
// Read 55: 1774
// Read 56: 1745
// Read 57: 1742
// Read 58: 1734
// Read 59: 1742
// Read 60: 1741
// Read 61: 1746
// Read 62: 1760
// Read 63: 1727
// Read 64: 1746
// Read 65: 1743
// Read 66: 1753
// Read 67: 1743
// Read 68: 1749
// Read 69: 1728
// Read 70: 1745
// Read 71: 1753
// Read 72: 1747
// Read 73: 1733
// Read 74: 1744
// Read 75: 1705
// Read 76: 1745
// Read 77: 1747
// Read 78: 1734
// Read 79: 1722
// Read 80: 1744
// Read 81: 1745
// Read 82: 1770
// Read 83: 1744
// Read 84: 1745
// Read 85: 1744
// Read 86: 1751
// Read 87: 1744
// Read 88: 1726
// Read 89: 1744
// Read 90: 1735
// Read 91: 1744
// Read 92: 1746
// Read 93: 1746
// Read 94: 1776
// Read 95: 1683
// Read 96: 1740
// Read 97: 1745
// Read 98: 1758
// Read 99: 1730
// Read 100: 1797

// === 100 reads complete. Pausing 5 seconds... ===

// Read 1: 1795
// Read 2: 1753
// Read 3: 1742
// Read 4: 1744
// Read 5: 1744
// Read 6: 1754
// Read 7: 1744
// Read 8: 1744
// Read 9: 1743
// Read 10: 1733
// Read 11: 1749
// Read 12: 1755
// Read 13: 1749
// Read 14: 1744
// Read 15: 1736
// Read 16: 1745
// Read 17: 1729
// Read 18: 1773
// Read 19: 1740
// Read 20: 1744
// Read 21: 1747
// Read 22: 1715
// Read 23: 1753
// Read 24: 1744
// Read 25: 1745
// Read 26: 1749
// Read 27: 1747
// Read 28: 1751
// Read 29: 1712
// Read 30: 1745
// Read 31: 1744
// Read 32: 1771
// Read 33: 1748
// Read 34: 1734
// Read 35: 1757
// Read 36: 1792
// Read 37: 1727
// Read 38: 1748
// Read 39: 1850
// Read 40: 1749
// Read 41: 1744
// Read 42: 1745
// Read 43: 1713
// Read 44: 1745
// Read 45: 1744
// Read 46: 1728
// Read 47: 1744
// Read 48: 1744
// Read 49: 1744
// Read 50: 1740
// Read 51: 1748
// Read 52: 1733
// Read 53: 1709
// Read 54: 1745
// Read 55: 1731
// Read 56: 1735
// Read 57: 1743
// Read 58: 1744
// Read 59: 1744
// Read 60: 1745
// Read 61: 1715
// Read 62: 1748
// Read 63: 1739
// Read 64: 1754
// Read 65: 1759
// Read 66: 1743
// Read 67: 1746
// Read 68: 1747
// Read 69: 1745
// Read 70: 1746
// Read 71: 1745
// Read 72: 1775
// Read 73: 1766
// Read 74: 1809
// Read 75: 1753
// Read 76: 1744
// Read 77: 1751
// Read 78: 1749
// Read 79: 1744
// Read 80: 1787
// Read 81: 1747
// Read 82: 1744
// Read 83: 1754
// Read 84: 1749
// Read 85: 1742
// Read 86: 1744
// Read 87: 1746
// Read 88: 1759
// Read 89: 1746
// Read 90: 1766
// Read 91: 1744
// Read 92: 1746
// Read 93: 1750
// Read 94: 1757
// Read 95: 1744
// Read 96: 1750
// Read 97: 1757
// Read 98: 1757
// Read 99: 1745
// Read 100: 1743

// === 100 reads complete. Pausing 5 seconds... ===

// Read 1: 1755
// Read 2: 1712
// Read 3: 1754
// Read 4: 1751
// Read 5: 1747
// Read 6: 1757
// Read 7: 1734
// Read 8: 1744
// Read 9: 1719
// Read 10: 1746
// Read 11: 1744
// Read 12: 1746
// Read 13: 1718
// Read 14: 1743
// Read 15: 1747
// Read 16: 1755
// Read 17: 1770
// Read 18: 1731
// Read 19: 1747
// Read 20: 1740
// Read 21: 1744
// Read 22: 1802
// Read 23: 1744
// Read 24: 1745
// Read 25: 1749
// Read 26: 1744
// Read 27: 1744
// Read 28: 1744
// Read 29: 1749
// Read 30: 1749
// Read 31: 1744
// Read 32: 1744
// Read 33: 1744
// Read 34: 1746
// Read 35: 1719
// Read 36: 1740
// Read 37: 1742
// Read 38: 1744
// Read 39: 1744
// Read 40: 1732
// Read 41: 1742
// Read 42: 1745
// Read 43: 1747
// Read 44: 1763
// Read 45: 1729
// Read 46: 1741
// Read 47: 1755
// Read 48: 1749
// Read 49: 1764
// Read 50: 1747
// Read 51: 1744
// Read 52: 1746
// Read 53: 1743
// Read 54: 1744
// Read 55: 1741
// Read 56: 1744
// Read 57: 1744
// Read 58: 1754
// Read 59: 1751
// Read 60: 1726
// Read 61: 1744
// Read 62: 1740
// Read 63: 1740
// Read 64: 1744
// Read 65: 1738
// Read 66: 1746
// Read 67: 1724
// Read 68: 1745
// Read 69: 1742
// Read 70: 1745
// Read 71: 1741
// Read 72: 1746
// Read 73: 1743
// Read 74: 1745
// Read 75: 1760
// Read 76: 1749
// Read 77: 1719
// Read 78: 1770
// Read 79: 1744
// Read 80: 1737
// Read 81: 1745
// Read 82: 1749
// Read 83: 1755
// Read 84: 1750
// Read 85: 1744
// Read 86: 1722
// Read 87: 1750
// Read 88: 1741
// Read 89: 1747
// Read 90: 1737
// Read 91: 1751
// Read 92: 1745
// Read 93: 1746
// Read 94: 1751
// Read 95: 1745
// Read 96: 1760
// Read 97: 1756
// Read 98: 1728
// Read 99: 1750
// Read 100: 1744

// === 100 reads complete. Pausing 5 seconds... ===

// Read 1: 1793
// Read 2: 1743
// Read 3: 1680
// Read 4: 1758
// Read 5: 1742
// Read 6: 1747
// Read 7: 1744
// Read 8: 1739
// Read 9: 1744
// Read 10: 1810
// Read 11: 1648
// Read 12: 1751
// Read 13: 1739
// Read 14: 1744
// Read 15: 1748
// Read 16: 1735
// Read 17: 1745
// Read 18: 1759
// Read 19: 1729
// Read 20: 1738
// Read 21: 1751
// Read 22: 1713
// Read 23: 1746
// Read 24: 1744
// Read 25: 1744
// Read 26: 1744
// Read 27: 1717
// Read 28: 1747
// Read 29: 1744
// Read 30: 1751
// Read 31: 1744
// Read 32: 1833
// Read 33: 1696
// Read 34: 1673
// Read 35: 1744
// Read 36: 1760
// Read 37: 1761
// Read 38: 1743
// Read 39: 1744
// Read 40: 1748
// Read 41: 1744
// Read 42: 1744
// Read 43: 1749
// Read 44: 1742
// Read 45: 1743
// Read 46: 1739
// Read 47: 1741
// Read 48: 1739
// Read 49: 1744
// Read 50: 1739
// Read 51: 1742
// Read 52: 1712
// Read 53: 1747
// Read 54: 1742
// Read 55: 1743
// Read 56: 1745
// Read 57: 1744
// Read 58: 1744
// Read 59: 1759
// Read 60: 1744
// Read 61: 1745
// Read 62: 1746
// Read 63: 1744
// Read 64: 1746
// Read 65: 1744
// Read 66: 1745
// Read 67: 1743
// Read 68: 1721
// Read 69: 1775
// Read 70: 1744
// Read 71: 1745
// Read 72: 1744
// Read 73: 1746
// Read 74: 1761
// Read 75: 1757
// Read 76: 1744
// Read 77: 1743
// Read 78: 1749
// Read 79: 1741
// Read 80: 1774
// Read 81: 1744
// Read 82: 1722
// Read 83: 1747
// Read 84: 1744
// Read 85: 1668
// Read 86: 1775
// Read 87: 1743
// Read 88: 1754
// Read 89: 1735
// Read 90: 1733
// Read 91: 1744
// Read 92: 1744
// Read 93: 1738
// Read 94: 1744
// Read 95: 1751
// Read 96: 1746
// Read 97: 1733
// Read 98: 1745
// Read 99: 1742
// Read 100: 1680

// === 100 reads complete. Pausing 5 seconds... ===

// Read 1: 1759
// Read 2: 1744
// Read 3: 1745
// Read 4: 1751
// Read 5: 1750
// Read 6: 1745
// Read 7: 1744
// Read 8: 1748
// Read 9: 1732
// Read 10: 1744
// Read 11: 1744
// Read 12: 1737
// Read 13: 1747
// Read 14: 1751
// Read 15: 1744
// Read 16: 1739
// Read 17: 1749
// Read 18: 1744
// Read 19: 1745
// Read 20: 1741
// Read 21: 1747
// Read 22: 1744
// Read 23: 1746
// Read 24: 1744
// Read 25: 1751
// Read 26: 1744
// Read 27: 1744
// Read 28: 1746
// Read 29: 1741
// Read 30: 1746
// Read 31: 1746
// Read 32: 1744
// Read 33: 1741
// Read 34: 1749
// Read 35: 1735
// Read 36: 1744
// Read 37: 1731
// Read 38: 1739
// Read 39: 1751
// Read 40: 1747
// Read 41: 1750
// Read 42: 1835
// Read 43: 1741
// Read 44: 1744
// Read 45: 1739
// Read 46: 1743
// Read 47: 1744
// Read 48: 1750
// Read 49: 1750
// Read 50: 1745
// Read 51: 1746
// Read 52: 1761
// Read 53: 1756
// Read 54: 1741
// Read 55: 1743
// Read 56: 1744
// Read 57: 1744
// Read 58: 1712
// Read 59: 1750
// Read 60: 1730
// Read 61: 1769
// Read 62: 1738
// Read 63: 1735
// Read 64: 1744
// Read 65: 1751
// Read 66: 1741
// Read 67: 1744
// Read 68: 1747
// Read 69: 1763
// Read 70: 1744
// Read 71: 1745
// Read 72: 1769
// Read 73: 1744
// Read 74: 1744
// Read 75: 1749
// Read 76: 1745
// Read 77: 1744
// Read 78: 1713
// Read 79: 1751
// Read 80: 1739
// Read 81: 1744
// Read 82: 1723
// Read 83: 1733
// Read 84: 1742
// Read 85: 1744
// Read 86: 1733
// Read 87: 1742
// Read 88: 1735
// Read 89: 1743
// Read 90: 1743
// Read 91: 1746
// Read 92: 1744
// Read 93: 1744
// Read 94: 1746
// Read 95: 1755
// Read 96: 1747
// Read 97: 1744
// Read 98: 1753
// Read 99: 1747
// Read 100: 1738

// === 100 reads complete. Pausing 5 seconds... ===

// Read 1: 1765
// Read 2: 1737
// Read 3: 1744
// Read 4: 1762
// Read 5: 1746
// Read 6: 1744
// Read 7: 1744
// Read 8: 1712
// Read 9: 1744
// Read 10: 1758
// Read 11: 1741
// Read 12: 1746
// Read 13: 1749
// Read 14: 1750
// Read 15: 1744
// Read 16: 1741
// Read 17: 1744
// Read 18: 1743
// Read 19: 1741
// Read 20: 1725
// Read 21: 1745
// Read 22: 1746
// Read 23: 1744
// Read 24: 1711
// Read 25: 1744
// Read 26: 1755
// Read 27: 1743
// Read 28: 1735
// Read 29: 1743
// Read 30: 1713
// Read 31: 1746
// Read 32: 1730
// Read 33: 1744
// Read 34: 1745
// Read 35: 1744
// Read 36: 1745
// Read 37: 1722
// Read 38: 1742
// Read 39: 1744
// Read 40: 1743
// Read 41: 1745
// Read 42: 1741
// Read 43: 1743
// Read 44: 1747
// Read 45: 1745
// Read 46: 1749
// Read 47: 1745
// Read 48: 1745
// Read 49: 1745
// Read 50: 1738
// Read 51: 1738
// Read 52: 1855
// Read 53: 1749
// Read 54: 1742
// Read 55: 1742
// Read 56: 1767
// Read 57: 1751
// Read 58: 1745
// Read 59: 1738
// Read 60: 1744
// Read 61: 1745
// Read 62: 1742
// Read 63: 1727
// Read 64: 1744
// Read 65: 1757
// Read 66: 1745
// Read 67: 1744
// Read 68: 1744
// Read 69: 1731
// Read 70: 1746
// Read 71: 1739
// Read 72: 1732
// Read 73: 1831
// Read 74: 1742
// Read 75: 1729
// Read 76: 1721
// Read 77: 1739
// Read 78: 1745
// Read 79: 1742
// Read 80: 1748
// Read 81: 1744
// Read 82: 1740
// Read 83: 1743
// Read 84: 1741
// Read 85: 1740
// Read 86: 1744
// Read 87: 1743
// Read 88: 1741
// Read 89: 1755
// Read 90: 1763
// Read 91: 1753
// Read 92: 1744
// Read 93: 1744
// Read 94: 1746
// Read 95: 1739
// Read 96: 1747
// Read 97: 1743
// Read 98: 1744
// Read 99: 1774
// Read 100: 1745

// === 100 reads complete. Pausing 5 seconds... ===

// Read 1: 1750
// Read 2: 1716
// Read 3: 1753
// Read 4: 1725
// Read 5: 1762
// Read 6: 1748
// Read 7: 1722
// Read 8: 1747
// Read 9: 1726
// Read 10: 1748
// Read 11: 1741
// Read 12: 1744
// Read 13: 1779
// Read 14: 1745
// Read 15: 1738
// Read 16: 1743
// Read 17: 1805
// Read 18: 1743
// Read 19: 1744
// Read 20: 1744
// Read 21: 1798
// Read 22: 1744
// Read 23: 1745
// Read 24: 1747
// Read 25: 1738
// Read 26: 1746
// Read 27: 1745
// Read 28: 1734
// Read 29: 1744
// Read 30: 1744
// Read 31: 1747
// Read 32: 1744
// Read 33: 1743
// Read 34: 1744
// Read 35: 1767
// Read 36: 1756
// Read 37: 1744
// Read 38: 1744
// Read 39: 1739
// Read 40: 1744
// Read 41: 1703
// Read 42: 1744
// Read 43: 1727
// Read 44: 1744
// Read 45: 1740
// Read 46: 1744
// Read 47: 1773
// Read 48: 1743
// Read 49: 1746
// Read 50: 1744
// Read 51: 1733
// Read 52: 1760
// Read 53: 1751
// Read 54: 1744
// Read 55: 1740
// Read 56: 1744
// Read 57: 1746
// Read 58: 1743
// Read 59: 1791
// Read 60: 1753
// Read 61: 1739
// Read 62: 1742
// Read 63: 1733
// Read 64: 1719
// Read 65: 1744
// Read 66: 1719
// Read 67: 1738
// Read 68: 1754
// Read 69: 1744
// Read 70: 1744
// Read 71: 1749
// Read 72: 1749
// Read 73: 1741
// Read 74: 1759
// Read 75: 1728
// Read 76: 1747
// Read 77: 1746
// Read 78: 1747
// Read 79: 1744
// Read 80: 1738
// Read 81: 1746
// Read 82: 1743
// Read 83: 1755
// Read 84: 1744
// Read 85: 1746
// Read 86: 1744
// Read 87: 1745
// Read 88: 1744
// Read 89: 1739
// Read 90: 1744
// Read 91: 1744
// Read 92: 1744
// Read 93: 1744
// Read 94: 1731
// Read 95: 1742
// Read 96: 1737
// Read 97: 1744
// Read 98: 1749
// Read 99: 1744
// Read 100: 1752

// === 100 reads complete. Pausing 5 seconds... ===

// Read 1: 1744
// Read 2: 1745
// Read 3: 1744
// Read 4: 1744
// Read 5: 1783
// Read 6: 1749
// Read 7: 1744
// Read 8: 1706
// Read 9: 1739
// Read 10: 1730
// Read 11: 1744
// Read 12: 1761
// Read 13: 1744
// Read 14: 1744
// Read 15: 1730
// Read 16: 1761
// Read 17: 1752
// Read 18: 1665
// Read 19: 1744
// Read 20: 1757
// Read 21: 1743
// Read 22: 1746
// Read 23: 1743
// Read 24: 1743
// Read 25: 1744
// Read 26: 1807
// Read 27: 1741
// Read 28: 1753
// Read 29: 1744
// Read 30: 1745
// Read 31: 1735
// Read 32: 1744
// Read 33: 1690
// Read 34: 1749
// Read 35: 1753
// Read 36: 1742
// Read 37: 1754
// Read 38: 1763
// Read 39: 1746
// Read 40: 1746
// Read 41: 1766
// Read 42: 1744
// Read 43: 1750
// Read 44: 1749
// Read 45: 1742
// Read 46: 1706
// Read 47: 1744
// Read 48: 1745
// Read 49: 1771
// Read 50: 1744
// Read 51: 1749
// Read 52: 1740
// Read 53: 1744
// Read 54: 1744
// Read 55: 1730
// Read 56: 1751
// Read 57: 1722
// Read 58: 1738
// Read 59: 1744
// Read 60: 1772
// Read 61: 1714
// Read 62: 1748
// Read 63: 1759
// Read 64: 1745
// Read 65: 1747
// Read 66: 1737
// Read 67: 1731
// Read 68: 1749
// Read 69: 1756
// Read 70: 1746
// Read 71: 1742
// Read 72: 1729
// Read 73: 1739
// Read 74: 1742
// Read 75: 1746
// Read 76: 1747
// Read 77: 1749
// Read 78: 1727
// Read 79: 1744
// Read 80: 1728
// Read 81: 1776
// Read 82: 1744
// Read 83: 1744
// Read 84: 1740
// Read 85: 1739
// Read 86: 1749
// Read 87: 1746
// Read 88: 1744
// Read 89: 1744
// Read 90: 1745
// Read 91: 1725
// Read 92: 1744
// Read 93: 1749
// Read 94: 1765
// Read 95: 1752
// Read 96: 1751
// Read 97: 1741
// Read 98: 1743
// Read 99: 1716
// Read 100: 1739

// === 100 reads complete. Pausing 5 seconds... ===

// Read 1: 1745
// Read 2: 1744
// Read 3: 1738
// Read 4: 1775
// Read 5: 1751
// Read 6: 1745
// Read 7: 1733
// Read 8: 1765
// Read 9: 1742
// Read 10: 1755
// Read 11: 1789
// Read 12: 1744
// Read 13: 1721
// Read 14: 1744
// Read 15: 1733
// Read 16: 1728
// Read 17: 1737
// Read 18: 1778
// Read 19: 1744
// Read 20: 1727
// Read 21: 1745
// Read 22: 1740
// Read 23: 1742
// Read 24: 1743
// Read 25: 1744
// Read 26: 1731
// Read 27: 1746
// Read 28: 1744
// Read 29: 1745
// Read 30: 1745
// Read 31: 1709
// Read 32: 1744
// Read 33: 1767
// Read 34: 1743
// Read 35: 1734
// Read 36: 1745
// Read 37: 1746
// Read 38: 1744
// Read 39: 1729
// Read 40: 1743
// Read 41: 1742
// Read 42: 1734
// Read 43: 1748
// Read 44: 1766
// Read 45: 1807
// Read 46: 1743
// Read 47: 1744
// Read 48: 1731
// Read 49: 1751
// Read 50: 1744
// Read 51: 1732
// Read 52: 1744
// Read 53: 1739
// Read 54: 1745
// Read 55: 1738
// Read 56: 1763
// Read 57: 1744
// Read 58: 1707
// Read 59: 1742
// Read 60: 1754
// Read 61: 1758
// Read 62: 1745
// Read 63: 1742
// Read 64: 1731
// Read 65: 1773
// Read 66: 1744
// Read 67: 1743
// Read 68: 1749
// Read 69: 1734
// Read 70: 1745
// Read 71: 1744
// Read 72: 1744
// Read 73: 1744
// Read 74: 1735
// Read 75: 1743
// Read 76: 1747
// Read 77: 1744
// Read 78: 1746
// Read 79: 1745
// Read 80: 1744
// Read 81: 1737
// Read 82: 1779
// Read 83: 1744
// Read 84: 1739
// Read 85: 1744
// Read 86: 1745
// Read 87: 1751
// Read 88: 1744
// Read 89: 1745
// Read 90: 1739
// Read 91: 1743
// Read 92: 1743
// Read 93: 1744
// Read 94: 1744
// Read 95: 1740
// Read 96: 1747
// Read 97: 1731
// Read 98: 1742
// Read 99: 1735
// Read 100: 1735

// === 100 reads complete. Pausing 5 seconds... ===

// Read 1: 1741
// Read 2: 1742
// Read 3: 1791
// Read 4: 1744
// Read 5: 1773
// Read 6: 1747
// Read 7: 1744
// Read 8: 1744
// Read 9: 1743
// Read 10: 1731
// Read 11: 1743
// Read 12: 1748
// Read 13: 1743
// Read 14: 1744
// Read 15: 1741
// Read 16: 1740
// Read 17: 1776
// Read 18: 1738
// Read 19: 1740
// Read 20: 1738
// Read 21: 1744
// Read 22: 1744
// Read 23: 1743
// Read 24: 1741
// Read 25: 1744
// Read 26: 1738
// Read 27: 1733
// Read 28: 1756
// Read 29: 1744
// Read 30: 1744
// Read 31: 1737
// Read 32: 1744
// Read 33: 1744
// Read 34: 1856
// Read 35: 1745
// Read 36: 1743
// Read 37: 1753
// Read 38: 1744
// Read 39: 1741
// Read 40: 1757
// Read 41: 1743
// Read 42: 1740
// Read 43: 1744
// Read 44: 1741
// Read 45: 1746
// Read 46: 1749
// Read 47: 1743
// Read 48: 1745
// Read 49: 1732
// Read 50: 1739
// Read 51: 1744
// Read 52: 1739
// Read 53: 1744
// Read 54: 1743
// Read 55: 1750
// Read 56: 1733
// Read 57: 1744
// Read 58: 1744
// Read 59: 1744
// Read 60: 1744
// Read 61: 1745
// Read 62: 1718
// Read 63: 1748
// Read 64: 1738
// Read 65: 1747
// Read 66: 1743
// Read 67: 1717
// Read 68: 1733
// Read 69: 1744
// Read 70: 1747
// Read 71: 1744
// Read 72: 1721
// Read 73: 1745
// Read 74: 1718
// Read 75: 1739
// Read 76: 1766
// Read 77: 1742
// Read 78: 1744
// Read 79: 1738
// Read 80: 1744
// Read 81: 1741
// Read 82: 1754
// Read 83: 1759
// Read 84: 1739
// Read 85: 1743
// Read 86: 1744
// Read 87: 1744
// Read 88: 1745
// Read 89: 1744
// Read 90: 1722
// Read 91: 1745
// Read 92: 1745
// Read 93: 1745
// Read 94: 1744
// Read 95: 1738
// Read 96: 1743
// Read 97: 1678
// Read 98: 1742
// Read 99: 1746
// Read 100: 1744

// === 100 reads complete. Pausing 5 seconds... ===

