using Gtk;
using Hildon;
using Dawg;
using Gee;

public class Updawg : Hildon.Program {
  private Hildon.Window window;
  private long *dawg;

  construct {
    long nedges;
    // initialize the dawg
    // c code: dawg_init(argv[1], &dawg, &nedges)
    Dawg.init("csw.dwg", out dawg, out nedges);

    window = new Hildon.Window ();
    window.destroy.connect (Gtk.main_quit);

    add_window (window);

    Environment.set_application_name ("UpDawg");

    var label  = new Gtk.Label ("UpDawg");
    var button = new Gtk.Button.with_label ("Anagrams");
    var output = new Gtk.TreeView ();
    var list_model = new ListStore (1, typeof (string));
    var input  = new Gtk.Entry ();
    var scroll = new ScrolledWindow (null, null);

    output.set_model(list_model);
    output.insert_column_with_attributes (-1, "Anagrams", new CellRendererText (), "text", 0);
    scroll.set_policy (PolicyType.AUTOMATIC, PolicyType.AUTOMATIC);
    scroll.add (output);

    button.clicked.connect (() => {
        if (input.text != null && input.text.len() > 0) {
          Gee.ArrayList<string> words = new Gee.ArrayList<string> ();
          Gee.ArrayList<string> blanks = new Gee.ArrayList<string> ();
          Dawg.anagrams(dawg, input.text, 1, words, blanks);
          label.set_text("%d words found".printf(words.size));
          TreeIter iter;
          list_model.clear();
          for (int i = 0; i < words.size; i++) {
            list_model.append(out iter);
            list_model.set(iter, 0, format(words[i], blanks[i]));
          }
        }
      });

    input.activate.connect(() => {
        label.set_text("enter pressed");
        button.clicked(); });

    var vbox = new Gtk.VBox (false, 2);

    vbox.pack_start (label, false, true, 2);
    vbox.pack_start (scroll, true, true, 2);
    vbox.pack_start (input, false, true, 2);
    vbox.pack_start (button, false, true, 2);

    window.add (vbox);
    input.grab_focus();
  }

  public void run () {
    window.show_all ();
    Gtk.main ();
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

    Gtk.init (ref args);

    var app = new Updawg ();
    app.run ();

    return(0);
  }
}
