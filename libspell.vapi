[CCode(cheader_filename = "splib.h", lower_case_cprefix = "dawg_", cprefix = "")]

namespace Dawg {
  public int init(string filename, out long* dictp, out long nedgesp);
	public void print(long *dawg, long node);
}
