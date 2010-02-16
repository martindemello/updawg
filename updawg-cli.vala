using Gee;
using DawgSearch;
using Readline;

public class UpdawgCli: Object {
  private DawgSearch dawg { private set; get; }

  construct {
    dawg = new DawgSearch("csw.dwg");
  }

  public void generate(string op, string pattern) {
    if (pattern != null && pattern.len() > 0) {
      var nwords = 0;
      switch(op) {
        case "Anagram":
          nwords = generate_anagram(pattern, 1); break;
        case "Pattern":
          nwords = generate_pattern(pattern); break;
        case "Build":
          nwords = generate_anagram(pattern, 0); break;
      }
      stdout.printf("%d words found\n", nwords);
    }
  }

  public int generate_anagram(string pattern, int full_rack) {
    Gee.ArrayList<string> words, blanks;
    dawg.anagram(pattern, full_rack, out words, out blanks);
    for (int i = 0; i < words.size; i++) {
      stdout.printf("%s\n", format(words[i], blanks[i]));
    }
    return words.size;
  }

  public int generate_pattern(string pattern) {
    Gee.Map ret = dawg.pattern(pattern);
    foreach (Gee.Map.Entry s in ret.entries) {
      stdout.printf("%s\n", format((string) s.key, (string) s.value));
    }
    return ret.size;
  }

  public void run () {
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

  public static void main(string[] args) {
    UpdawgCli up = new UpdawgCli();
    while (true) {
      var rack = Readline.readline ("anag> ");
      if (rack != null && rack!= "") {
        up.generate("Anagram", rack);
        Readline.History.add (rack);
      }
    }
  }
}
