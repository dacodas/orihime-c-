#include <sstream>

#include "grpc.h"
#include "util.h"

#include "add.h"

void AddWord(int argc, char** argv)
{
    // ?? This will probably not be used often
}

void AddSource(int argc, char** argv)
{
    // orihime add source 読売新聞
    std::string source {argv[3]};
    client.AddSource(source);
}

void AddText(int argc, char** argv)
{
    // cat <<EOF | orihime add text --source 朝日新聞
    // 標津町の男性職員（当時２４）が過度の時間外労働による心理的負担から昨年７月に自殺したと結論づけた第三者調査結果は「同町で多くの職員の長時間勤務が常態化している様子がうかがわれる」と指摘。町側に勤務状況の早急な改善を強くうながした。背景には、北方領土問題の啓発事業の一環として同町はじめ周辺自治体を訪れる修学旅行の急増など、地域特有の要因もある。
    // EOF
    // {"-s", "--source"}

    // cat <<EOF | orihime add text --most-recent-source
    // あゝ、考へても怖ろしい……と云つてさつぱりと行つてしまつた程なのだから無論此方のことなどを思ひ出すことなどはいつになつたつてありやあしまい――。
    // EOF
    // {"-r", "--most-recent-source"}

    // Set source
    std::string source;
    if ( !std::string{"--source"}.compare(argv[3]) or
         !std::string{"-s"}.compare(argv[3]) ) {
        source = argv[4];
    }
    else if ( !std::string{"--most-recent-source"}.compare(argv[3]) or
              !std::string{"-r"}.compare(argv[3]) ) {
        // source.set_name(most_recent_source());
        std::cerr << "--most-recent-source not implemented!\n";
        exit(1);
    }
    else {
        std::cerr << "Expecting a source!\n";
        exit(1);
    }

    // Test for numeric argument
    int source_id {0};
    try {
        source_id = std::stoi(source);
    }
    catch (const std::invalid_argument& e ) {
        std::cerr << "Only numeric source IDs allowed for now \n";
        exit(1);
    }

    std::stringstream stdin_stringstream {};
    stdin_stringstream << std::cin.rdbuf();
    std::string content {stdin_stringstream.str()};
    client.AddText(source_id, content);
}

void AddChildWord(int argc, char** argv)
{
    std::vector<std::string> arguments(argv, argv + argc);
    typedef std::vector<std::string>::iterator arg;

    arg pa = std::find(arguments.begin(), arguments.end(), "--parent-text");
    arg p = std::find(arguments.begin(), arguments.end(), "-p");
    arg so = std::find(arguments.begin(), arguments.end(), "--source");
    arg s = std::find(arguments.begin(), arguments.end(), "-s");
    arg wo = std::find(arguments.begin(), arguments.end(), "--word");
    arg w = std::find(arguments.begin(), arguments.end(), "-w");

    // uint64_t user_id = config.user_id();
    uint64_t user_id = 1;

    if ( ( pa == arguments.end() and p == arguments.end() ) or
         ( so == arguments.end() and s == arguments.end() ) or
         ( wo == arguments.end() and w == arguments.end() ) )
    {
        std::cerr << "Missing one of --parent-text, --source, or --word\n";
        exit(1);
    }

    // TODO: Check bounds
    std::string parent = pa == arguments.end() ? *(++p) : *(++pa);
    std::string source = so == arguments.end() ? *(++s) : *(++so);
    std::string word = wo == arguments.end() ? *(++w) : *(++wo);

    std::vector<char> parent_bytes = HexToBytes(parent);

    int source_id {0};
    try {
        source_id = std::stoi(source);
    }
    catch (const std::invalid_argument& e ) {
        std::cerr << "Only numeric source IDs allowed for now \n";
        exit(1);
    }

    std::stringstream stdin_stringstream {};
    stdin_stringstream << std::cin.rdbuf();
    std::string definition {stdin_stringstream.str()};

    client.AddChildWord(user_id, parent_bytes, source_id, word, definition);

    // This source is the definition source, shouldn't update the --most-recent-source used in AddText
    // cat <<EOF | orihime add child-word --parent-text 1b2cb68e656c14672c32344d9b3be8ef --source goo辞書 --word 労働
    // <!DOCTYPE html>
    // <html>
    // <head>
    //   <meta name="generator" content=
    //   "HTML Tidy for HTML5 for Linux version 5.6.0">
    //   <title></title>
    // </head>
    // <body>
    //   <div class="l-pgTitle">
    //     <h1>ろう‐どう〔ラウ‐〕【労働】<span class="meaning">の意味</span></h1><a href=
    //     "javascript:void(0);" class=
    //     "l-pgTitle-bkmkSaveBtn c-rollover is-newAttentionView">ブックマークへ登録</a>
    //   </div>
    //   <div class="contents_area meaning_area cx">
    //     <p class="source mb8">出典：<a href=
    //     "/jn/"><cite>デジタル大辞泉</cite></a><a href=
    //     "http://www.daijisen.jp/"><cite>（小学館）</cite></a></p>
    //     <div class="contents">
    //       <div class="text">
    //         <span class="hinshi">［名］</span><span class=
    //         "hinshi">(スル)</span>
    //       </div>
    //       <ol class="meaning cx">
    //         <li>
    //           <!-- l-ol-->
    //           <p class="text"><strong>１</strong>
    //           からだを使って働くこと。特に、収入を得る目的で、からだや知能を使って働くこと。「工場で労働する」「時間外労働」「頭脳労働」</p>
    //         </li>
    //       </ol>
    //       <ol class="meaning cx">
    //         <li>
    //           <p class="text"><strong>２</strong> <a href=
    //           "/word/%E7%B5%8C%E6%B8%88%E5%AD%A6/#jn-66339">経済学</a>で、生産に向けられる人間の努力ないし活動。自然に働きかけてこれを変化させ、<a href="/word/%E    7%94%9F%E7%94%A3%E6%89%8B%E6%AE%B5/#jn-121586">生産手段</a>や生活手段をつくりだす人間の活動。労働力の使用・消費。</p>
    //         </li>
    //       </ol>
    //     </div>
    //   </div>
    // </body>
    // </html>
    // EOF
}
