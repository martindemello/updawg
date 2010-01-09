using Dawg;

public class Program {
	public static int main(string[] args) {
	  long *dawg;
   	long nedges;
		long root_node;
		root_node = 1;

		// c code: dawg_init(argv[1], &dawg, &nedges)
		Dawg.init("csw.dwg", out dawg, out nedges);
    Dawg.print(dawg, root_node);
		return 0;
	}
}
