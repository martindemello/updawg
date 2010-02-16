using Dawg;
using Gee;

class DawgSearch: Object {
  public string dawgfile { private get; construct; }
  private long *dawg;

  public DawgSearch(string dawgfile) {
    Object(dawgfile: dawgfile);
  }

  construct {
    long nedges;
    Dawg.init(dawgfile, out dawg, out nedges);
  }

  public void anagram(string rack, int full_rack, out Gee.ArrayList<string> words, out Gee.ArrayList<string> blanks) {
    words = new Gee.ArrayList<string> ();
    blanks = new Gee.ArrayList<string> ();
    Dawg.anagrams(dawg, rack, full_rack, words, blanks);
  }

  public Gee.Map pattern(string rack) {
    Gee.Map ret = new Gee.TreeMap<string, string> ();
    Dawg.wildcard(dawg, rack, ret);
    return ret;
  }
}
