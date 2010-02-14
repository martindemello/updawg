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

    var label  = new Gtk.Label ("");
    var button = new Gtk.Button.with_label ("Search");
    var output = new Gtk.TreeView ();
    var list_model = new ListStore (1, typeof (string));
    var input  = new Hildon.Entry (Hildon.SizeType.AUTO);
    var scroll = new Hildon.PannableArea ();
    var op = new Hildon.TouchSelector.text ();
    var pick = new Hildon.PickerButton (Hildon.SizeType.AUTO, //_WIDTH | Hildon.SizeType.FINGER_HEIGHT,
        Hildon.ButtonArrangement.HORIZONTAL);

    op.append_text("Anagram");
    op.append_text("Pattern");
    op.append_text("Build");
    pick.set_selector(op);
    pick.set_active(0);

    input.set("hildon-input-mode", Hildon.GtkInputMode.FULL);
    output.set_model(list_model);
    output.insert_column_with_attributes (-1, "Anagrams", new CellRendererText (), "text", 0);
    scroll.mov_mode = Hildon.MovementMode.VERT;
    scroll.add_with_viewport (output);


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
    var hbox = new Gtk.HBox (false, 2);
    hbox.pack_start (input, true, true, 2);
    hbox.pack_start (pick, false, true, 2);
    hbox.pack_start (button, false, true, 2);

    vbox.pack_start (label, false, true, 2);
    vbox.pack_start (scroll, true, true, 2);
    vbox.pack_start (hbox, false, true, 2);

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
