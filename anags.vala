using Dawg;
using Gee;

public class Program {
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
	  long *dawg;
   	long nedges;
		long root_node;
		root_node = 1;

		// c code: dawg_init(argv[1], &dawg, &nedges)
		Dawg.init("csw.dwg", out dawg, out nedges);
    string rackString = args[1];

    Gee.ArrayList<string> output = new Gee.ArrayList<string> ();
    Gee.ArrayList<string> blanks = new Gee.ArrayList<string> ();
    Dawg.anagrams(dawg, rackString, 1, output, blanks);
    for (int i = 0; i < output.size; i++) {
      stdout.printf("%s\n", format(output[i], blanks[i]));
    }

    
    //Dawg.print(dawg, root_node);
		return(0);
	}
}
