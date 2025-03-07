#include "config.h"

Config::Config(QObject *parent)
    : QObject{parent}
{}

QStringList Config::getSentences() const {
    return {
        "Habt ihr was im Kaffee gehabt",
        "Random Fact",
        "Blätter mit iPad fotografieren",
        "Wollt ihr mal vor die tür gehen",
        "Das ist keine Akkordarbeit",
        "Falsche namens aussprache 'Danaeee'",
        "Rumgehen",
        "Timo Richard Cameo",
        "Drucken",
        "Rassistische Bemerkung",
        "Atmen",
        "Frag ob wir x wissen obwohl wir nichts gemacht hatten",
        "Free!",
        "Böhmer geht in den Bastellraum",
        "Studienfakten",
        "Was habt ihr raus?",
        "Wollt ihr mich etwa provozieren",
        "Transistoren rumgeben",
        "1 Blatt",
        "2 Blätter",
        "4 Blätter",
        "6 Blätter",
        "Hört es euch doch mit kopfhöhrern an",
        "Dominik und Sergej durchbrechen die Schallwellen",
        "Hohe Versprechen",
        "BOM zweifelt an seinem Talent",
        "Sauklaue",
        "Merkt euch das",
        "Außversehen iPad Blatt wechseln beim schreiben",
        "Danylo hat einen 'Termin'",
        "Soweit verstanden?",
        "Fragt, wenn ihr etwas nicht verstanden habt.",
        "Wenn ihr das Blatt nicht braucht, könnt ihr mir das auch zurück geben.",
        "Random Sprichwort",
        "In Prinzip",
        "BOM bleibt still für über eine Minute",
        "Lange pausen zwischen Sätzen"
    };
}
