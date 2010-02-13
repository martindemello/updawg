using Gee;

[CCode(cheader_filename = "splib.h", lower_case_cprefix = "dawg_", cprefix = "")]
[CCode(cheader_filename = "dawgutils.h", lower_case_cprefix = "dawg_", cprefix = "")]

namespace Dawg {
  public int init(string filename, out long* dictp, out long nedgesp);
  public void print(long *dawg, long node);
  public void anagrams(long *dawg, string rack, int bingosOnly, Gee.ArrayList* retwords, Gee.ArrayList* retblanks);
  public void wildcard(long *dawg, string word, Gee.Map* ret);
}
