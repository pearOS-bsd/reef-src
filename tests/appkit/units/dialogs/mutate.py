# In the container, after build.sh and run.sh: each small change of the source must show in the log (is the test blind to something?).
import subprocess, os, sys, concurrent.futures as cf
M=[('drag','std::exchange(provide, nullptr)','provide'),
('drag','data ? size : 0','size'),
('drag','if (release)\n            release(context);',';'),
('drag','std::exchange(windowDragEnded, nullptr)','windowDragEnded'),
('drag','if (type && name == type)','if (type && name.size() == strlen(type))'),
('drag','std::min(size, bytes.size())','std::min(size, bytes.size() + 1)'),
('drag','types += type + "\\n"','types += type + " "'),
('drag','callback(context, data.data(), data.size(), 1);','callback(context, data.data(), data.size(), 0);'),
('drag','return callback(context, nullptr, 0, 0);','return;'),
('drag','items[i].type ? items[i].type : ""','items[i].type ? items[i].type : "x"'),
('drag','offer.actions = actions ? actions : ui::CopyAction;','offer.actions = actions;'),
('drag','ui::TestInput::dragMove(target, {x, y});',''),
('drag','done(data[i]);\n                return;','done(data[i]);'),
('drag','if (items[i].provide) {','if (items[i].provide && !items[i].bytes) {'),
('drag','item->release = items[i].release;',''),
('drag','if (items[i].type)\n            data','if (items[i].type && *items[i].type)\n            data'),
('drag','dragItems = *data;',''),
('drag','if (!pixels || width <= 0 || height <= 0)','if (!pixels || width <= 1 || height <= 0)'),
('drag','scale > 0 ? scale : 1','scale >= 0 ? scale : 1'),
('drag','source.hotSpot = {hotX, hotY};','source.hotSpot = {hotY, hotX};'),
('drag','size_t(width) * 4','size_t(height) * 4'),
('drag','source.actions = actions ? actions : ui::CopyAction;','source.actions = actions;'),
('drag','dragActions = source.actions;','dragActions = actions;'),
('drag','source.actions = ui::MoveAction;',''),
('drag','dragged ? dragged->window : nullptr','nullptr'),
('drag','source.attachedOffset = {offsetX, offsetY};',''),
('drag','finished(context, action, dropped);','finished(context, action, !dropped);'),
('drag','if (headless) {\n        windowDragEnded = std::move(source.onEnded);\n        window->root->pressed = false;','if (headless) {\n        windowDragEnded = std::move(source.onEnded);'),
('drag','return 1;\n    window->root->pressed = false;\n    return 0;\n}\n\n// 249','return 1;\n    return 0;\n}\n\n// 249'),
('dialogs','optionPointers.push_back(it == chosen.end() ? nullptr : it->second.c_str());','if (it != chosen.end()) optionPointers.push_back(it->second.c_str());'),
('dialogs','option.first == id','option.second == id'),
('dialogs','if (!part.empty())\n                parts',' parts'),
('dialogs',"*text == ';' || !*text","*text == ',' || !*text"),
('dialogs','part.clear();',''),
('dialogs','mode == 2 ? ui::FileChooser::Mode::OpenFolder','mode == 2 ? ui::FileChooser::Mode::OpenFile'),
('dialogs',': ui::FileChooser::Mode::OpenFile);',': ui::FileChooser::Mode::OpenFiles);'),
('dialogs','testPaths.clear();',''),
('dialogs','testOptions.clear();',''),
('dialogs',"option.find('=')","option.rfind('=')"),
('dialogs','option.substr(equals + 1)','option.substr(equals)'),
('dialogs','hasTestAnswer = true;',''),
('dialogs','hasTestAnswer = false;\n        base',' base'),
('dialogs','i < choiceCount','i + 1 < choiceCount'),
('dialogs','choice.selected = choices[i].selected ? choices[i].selected : "";',''),
('dialogs','selected.emplace_back(choice.id, choice.selected);','selected.emplace_back(choice.id, "");'),
('dialogs','value = option;','{ value = option; break; }'),
('dialogs','paths.empty() ? Options() : chosen','chosen'),
('dialogs','if (*alive)\n                answer','answer'),
('dialogs','window ? window->window : nullptr','nullptr'),
('dialogs','if (title && *title)','if (title)'),
('dialogs','if (acceptLabel && *acceptLabel)','if (acceptLabel)'),
('dialogs','if (folder && *folder)','if (folder)'),
('dialogs','if (file && *file)','if (file)'),
('dialogs','filter.mimeTypes = split(filters[i].mimeTypes);','filter.mimeTypes = split(filters[i].patterns);'),
('dialogs','if (!filterList.empty())',''),
('dialogs','if (!choiceList.empty())',''),
('dialogs','dialog->setModal(modal);','dialog->setModal(modal == 1);'),
('dialogs','answer(callback, context, {}, ids, {});','answer(callback, context, {}, {}, {});'),
('dialogs','*chooser->alive = false;',''),
('dialogs','if (!chooser)\n        return;',''),
]
src={u:open('/w/src/appkit-host/%s.cpp'%u).read() for u in ('drag','dialogs')}
F="-std=c++20 -fPIC -w -O0 -fvisibility=hidden -I/tk/tk7/src -I/tk/app/stubs -I/w/src/appkit-host -I/usr/include/freetype2 -I/usr/include/harfbuzz".split()
T='/tmp/dialogs/t/'
base=open(T+'orig.log','rb').read()
def one(n):
    u,a,b=M[n]
    if a not in src[u]: return n,'NOT FOUND'
    d='/tmp/dialogs/m/%d/'%n; os.makedirs(d,exist_ok=True)
    open(d+'v.cpp','w').write(src[u].replace(a,b,1))
    r=subprocess.run(['g++']+F+['-c','-o',d+'v.o',d+'v.cpp'],capture_output=True,text=True)
    if r.returncode: return n,'ERROR '+r.stderr[:300]
    other={'drag':'dialogs','dialogs':'drag'}[u]
    r=subprocess.run(['g++','-shared','-Wl,-z,lazy','-o',d+'mine.so',d+'v.o',T+other+'.o',T+'shim.o','/root2/libcutefish-toolkit.so.1','/root2/libcutefish-toolkit-base.so.1','-Wl,-rpath-link,/root2'],capture_output=True,text=True)
    if r.returncode: return n,'LINK '+r.stderr[:300]
    e=dict(os.environ,LD_LIBRARY_PATH='/root2',HOME=T+'home',XDG_RUNTIME_DIR=T+'home',LC_ALL='C.UTF-8',LD_PRELOAD=T+'hooks.so',MINE=d+'mine.so')
    try: p=subprocess.run([T+'driver','ours'],capture_output=True,env=e,timeout=200)
    except subprocess.TimeoutExpired: return n,'seen (timeout)'
    if p.returncode: return n,'seen (exit %d)'%p.returncode
    return n,'BLIND' if p.stdout==base else 'seen'
with cf.ThreadPoolExecutor(int(os.environ.get('J','3'))) as ex:
    res=list(ex.map(one,[int(x) for x in sys.argv[1:]] or range(len(M))))
for n,r in res:
    if r!='seen': print(n,r,M[n])
print(sum(r.startswith('seen') for _,r in res),'of',len(res),'seen')
