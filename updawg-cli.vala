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
        case "a":
          nwords = generate_anagram(pattern, 1); break;
        case "p":
          nwords = generate_pattern(pattern); break;
        case "b":
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

  public string get_prompt(string op) {
    switch (op) {
      case "a": return "anagram> ";
      case "p": return "pattern> ";
      case "b": return "build> ";
    }
    return "error> ";
  }
    
  public void run() {
    var op = "a";
    var prompt = get_prompt(op);
    string x;
    while (true) {
      var rack = Readline.readline (prompt);
      if (rack in "Qq") { break; }
      if (rack != null && rack.len() > 0) {
        if (rack.substring(1,1) == " ") {
          x = rack.substring(0, 1);
          if (x in "abp") {
            op = x;
            prompt = get_prompt(op);
          }
          rack = rack.substring(2);
        }

        if (rack != null && rack.len() > 0) {
          generate(op, rack);
        }

        Readline.History.add (rack);
      }
    }
  }

  public static void main(string[] args) {
    UpdawgCli up = new UpdawgCli();
    up.run();
  }
}
