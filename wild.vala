using Dawg;
using Gee;

public class Updawg : Object {
  private long *dawg;

  construct {
    long nedges;
    // initialize the dawg
    // c code: dawg_init(argv[1], &dawg, &nedges)
    Dawg.init("csw.dwg", out dawg, out nedges);
  }

  public Map wildcard(string word) {
    Gee.Map retlist = new Gee.TreeMap<string, string> ();
    Dawg.wildcard(dawg, word, retlist);
    return retlist;
  }

  public static string format(string word, string blanks) {
    var l = word.len();
    var s = new StringBuilder();
    for (int i = 0; i < l; i++) {
      var x = (char) word[i];
      int b = (int) blanks[i];

      if (b == 1) {
        s.append_unichar(x.toupper());
      } else {
        s.append_unichar(x);
      }
    }

    return s.str;
  }

  public static int main(string[] args) {
    

    //Gee.ArrayList<string> output = new Gee.ArrayList<string> ();
    //Gee.ArrayList<string> blanks = new Gee.ArrayList<string> ();
    //Dawg.anagrams(dawg, rackString, 1, output, blanks);
    //for (int i = 0; i < output.size; i++) {
    //  stdout.printf("%s\n", format(output[i], blanks[i]));
    //}

    var app = new Updawg ();
    var ret = app.wildcard("h?ll*");
    foreach (string s in (Set<string>)ret.keys) {
      stdout.printf ("%s\n", format(s, (string)ret[s]));
    }

    return(0);
  }
}
