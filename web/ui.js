(function(){
'use strict';

var D = window.DATA || { types: [], asms: [], game: 'UNKNOWN', gen: '' };
var T = D.types || [];
var $ = function(s){ return document.querySelector(s); };
var $$ = function(s){ return document.querySelectorAll(s); };

function esc(s){
  return String(s).replace(/[&<>"']/g, function(c){
    return {'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c];
  });
}

function norm(s){
  return String(s).toLowerCase().replace(/[^\p{L}\p{N}]/gu, '');
}

/* ---------------- LOCALIZATION (STRICT OSINT TERMINAL) ---------------- */
var L = {
  ru: {
    ph: 'QUERY:: Поиск классов, полей, свойств, функций, сигнатур…',
    bool: 'FLAGS', num: 'STATS', other: 'OBJECTS', method: 'FUNCS',
    pub: '[PUBLIC]', stat: '[STATIC]', cst: '[CONST]',
    allNs: '[ALL_NAMESPACES]', allAsm: '[ALL_ASSEMBLIES]',
    types: 'types', found: 'FOUND', inCls: 'IN_CLASSES', classes: 'CLASSES',
    more: '[EXPAND_MORE]', rowsMore: '[EXPAND_MORE]',
    tryIt: 'SUGGESTED_QUERIES', allBool: 'ALL_FLAGS (bool)', allNum: 'ALL_STATS (num)', allFn: 'ALL_FUNCS',
    nsTitle: 'NAMESPACES_MATRIX',
    back: '[← BACK_TO_INDEX]', howGet: 'TARGET_INSTANCE_ACQUISITION',
    values: 'VALUES_AND_FLAGS', fields: 'FIELDS', props: 'PROPERTIES',
    methods: 'FUNCTIONS', events: 'EVENTS', enumVals: 'ENUM_CONSTANTS',
    inherit: '[INCLUDE_INHERITED]', filter: 'FILTER_BY_IDENTIFIER…',
    ext: 'EXTENDS', derived: 'DERIVED', cName: 'NAME', cCs: 'C#', cH: 'HARMONY',
    copied: '[COPIED_TO_CLIPBOARD]', none: '[ERR: ZERO_MATCHES_FOUND]', ro: 'RO',
    welcome: 'DNMETA // OSINT MEMORY ENGINE',
    welcomeSub: 'Терминал анализа бинарных метаданных Mono и структуры целевого процесса.',
    noHolder: 'Статических ссылок не обнаружено — требуется получение объекта через сцену или вызовы.',
    unity: 'UnityEngine.Object на сцене (FindObjectOfType)', members: 'members',
    statics: 'static — глобальная ссылка без инстанса',
    totalT: 'TYPES_AND_STRUCTS', totalB: 'BOOL_FLAGS', totalN: 'NUM_STATS', totalF: 'FUNCTIONS',
    collection: 'ARRAY_COLLECTION', hintPriv: 'private — требуется AccessTools', nothing: '[EMPTY]',
    tabGraph: 'NEURAL_MAP', tabExplorer: 'DIRECTORY_INDEX', tabCheats: 'EXPLOIT_HOOKS', tabModules: 'NAMESPACES',
    graphHint: 'Drag to move • Double-click node to expand • Double-click background to go back',
    levelUp: '[← LEVEL_UP]', rootLevel: 'ROOT_KERNEL',
    inspectClass: '[INSPECT_TYPE]',
    cheatHubTitle: 'EXPLOIT_HOOKS // CRITICAL TARGET VARIABLES',
    cheatHubSub: 'Реестр ключевых игровых переменных (Godmode, Ammo, Speed, Health, Currencies).',
    cheatBools: 'BOOLEAN_FLAGS (GODMODE, AMMO, NOCLIP)',
    cheatNums: 'NUMERIC_STATS (HEALTH, SPEED, CURRENCIES)',
    cheatMethods: 'CRITICAL_HOOKS (KILL, DAMAGE, TELEPORT, SPAWN)',
    hubBools: 'FLAGS::BOOL', hubNums: 'STATS::NUMERIC',
    hubMethods: 'FUNCS::HOOKS', hubStatics: 'SYS::SINGLETONS'
  },
  en: {
    ph: 'QUERY:: Search types, fields, properties, functions, signatures…',
    bool: 'FLAGS', num: 'STATS', other: 'OBJECTS', method: 'FUNCS',
    pub: '[PUBLIC]', stat: '[STATIC]', cst: '[CONST]',
    allNs: '[ALL_NAMESPACES]', allAsm: '[ALL_ASSEMBLIES]',
    types: 'types', found: 'FOUND', inCls: 'IN_CLASSES', classes: 'CLASSES',
    more: '[EXPAND_MORE]', rowsMore: '[EXPAND_MORE]',
    tryIt: 'SUGGESTED_QUERIES', allBool: 'ALL_FLAGS (bool)', allNum: 'ALL_STATS (num)', allFn: 'ALL_FUNCS',
    nsTitle: 'NAMESPACES_MATRIX',
    back: '[← BACK_TO_INDEX]', howGet: 'TARGET_INSTANCE_ACQUISITION',
    values: 'VALUES_AND_FLAGS', fields: 'FIELDS', props: 'PROPERTIES',
    methods: 'FUNCTIONS', events: 'EVENTS', enumVals: 'ENUM_CONSTANTS',
    inherit: '[INCLUDE_INHERITED]', filter: 'FILTER_BY_IDENTIFIER…',
    ext: 'EXTENDS', derived: 'DERIVED', cName: 'NAME', cCs: 'C#', cH: 'HARMONY',
    copied: '[COPIED_TO_CLIPBOARD]', none: '[ERR: ZERO_MATCHES_FOUND]', ro: 'RO',
    welcome: 'DNMETA // OSINT MEMORY ENGINE',
    welcomeSub: 'Terminal analysis of Mono metadata and target binary memory layout.',
    noHolder: 'No static references found - acquire instance via scene search.',
    unity: 'UnityEngine.Object in active scene (FindObjectOfType)', members: 'members',
    statics: 'static - usable without object instance',
    totalT: 'TYPES_AND_STRUCTS', totalB: 'BOOL_FLAGS', totalN: 'NUM_STATS', totalF: 'FUNCTIONS',
    collection: 'ARRAY_COLLECTION', hintPriv: 'private - reflection required (AccessTools)', nothing: '[EMPTY]',
    tabGraph: 'NEURAL_MAP', tabExplorer: 'DIRECTORY_INDEX', tabCheats: 'EXPLOIT_HOOKS', tabModules: 'NAMESPACES',
    graphHint: 'Drag to move • Double-click node to expand • Double-click background to go back',
    levelUp: '[← LEVEL_UP]', rootLevel: 'ROOT_KERNEL',
    inspectClass: '[INSPECT_TYPE]',
    cheatHubTitle: 'EXPLOIT_HOOKS // CRITICAL TARGET VARIABLES',
    cheatHubSub: 'Registry of key gameplay variables (Godmode, Ammo, Speed, Health, Currencies).',
    cheatBools: 'BOOLEAN_FLAGS (GODMODE, AMMO, NOCLIP)',
    cheatNums: 'NUMERIC_STATS (HEALTH, SPEED, CURRENCIES)',
    cheatMethods: 'CRITICAL_HOOKS (KILL, DAMAGE, TELEPORT, SPAWN)',
    hubBools: 'FLAGS::BOOL', hubNums: 'STATS::NUMERIC',
    hubMethods: 'FUNCS::HOOKS', hubStatics: 'SYS::SINGLETONS'
  }
};

var lang = (function(){
  try { return localStorage.getItem('dnl'); } catch(e){ return null; }
})() || 'en';

function tr(k){ return (L[lang] && L[lang][k]) || k; }

var ACC = { 6:'public', 4:'protected', 5:'protected internal', 3:'internal', 2:'private protected', 1:'private', 0:'private' };
function isPub(a){ return a === 6 || a === 4 || a === 5; }
function strip(s){ return String(s).replace(/<.*$/, ''); }
function short(s){ return String(s).replace(/(?:[A-Za-z_][\w]*\.)+(?=[A-Za-z_<])/g, ''); }
function hum(n){
  var s = String(n).replace(/^(m_|s_|_+)/, '').replace(/_+/g, ' ');
  s = s.replace(/([a-z0-9])([A-Z])/g, '$1 $2').replace(/([A-Z]+)([A-Z][a-z])/g, '$1 $2').trim();
  return s ? s.toUpperCase() : String(n);
}

var NUM = {};
['int','uint','long','ulong','short','ushort','byte','sbyte','float','double','System.Decimal','IntPtr','UIntPtr'].forEach(function(x){ NUM[x] = 1; });

var byFull = new Map();
T.forEach(function(x, i){
  x.id = i;
  x._k = norm(x.n);
  if(!byFull.has(x.f)) byFull.set(x.f, x);
});

function cat(ts){
  if(ts === 'bool') return 'bool';
  if(NUM[ts]) return 'num';
  if(ts === 'string' || ts === 'char') return 'text';
  var b = byFull.get(strip(ts));
  if(b && b.k === 'enum') return 'enum';
  return 'other';
}

var ORDER = { bool: 0, num: 1, text: 2, enum: 3, other: 4, event: 5, method: 6 };
var staticBy = new Map(), nsCount = {}, totals = { t: T.length, b: 0, n: 0, f: 0 };

T.forEach(function(tp){
  var m = [];
  (tp.F || []).forEach(function(f){
    m.push({ o: tp, kind: 'field', n: f.n, ty: f.t, a: f.a, s: !!f.s, ro: !!f.r, c: f.c, at: f.at, cat: cat(f.t) });
  });
  (tp.P || []).forEach(function(p){
    m.push({ o: tp, kind: 'prop', n: p.n, ty: p.t, a: p.a, s: !!p.s, g: p.g, w: p.w, ix: p.ix, at: p.at, cat: cat(p.t) });
  });
  (tp.E || []).forEach(function(e){
    m.push({ o: tp, kind: 'event', n: e.n, ty: e.t, a: e.a, s: !!e.s, at: e.at, cat: 'event' });
  });
  (tp.M || []).forEach(function(f){
    m.push({ o: tp, kind: 'method', n: f.n, r: f.r, p: f.p || [], a: f.a, s: !!f.s, mod: f.m, gp: f.g, at: f.at, cat: 'method' });
  });

  m.forEach(function(x, i){
    x.k = norm(x.n);
    x.i = i;
    if(x.c === undefined){
      if(x.cat === 'bool') totals.b++;
      else if(x.cat === 'num') totals.n++;
      else if(x.cat === 'method') totals.f++;
    }
    if(x.s && x.c === undefined && (x.kind === 'field' || x.kind === 'prop')){
      var key = strip(x.ty), coll = false, mm;
      if((mm = /^(.*)\[\]$/.exec(x.ty))){ key = strip(mm[1]); coll = true; }
      else if((mm = /<([^<>,]+)>$/.exec(x.ty))){ key = strip(mm[1]); coll = true; }
      if(!staticBy.has(key)) staticBy.set(key, []);
      staticBy.get(key).push({ m: x, coll: coll });
    }
  });

  tp.mem = m;
  nsCount[tp.ns] = (nsCount[tp.ns] || 0) + 1;
});

var NSLIST = Object.keys(nsCount).sort(function(a, b){ return a.localeCompare(b); });

/* ---------------- STATE ---------------- */
var S = {
  nav: 'graph',
  q: '',
  chips: { bool: true, num: true, other: true, method: true },
  ns: null,
  asm: -1,
  pub: false,
  stat: false,
  cst: false,
  limit: 30,
  exp: {},
  tab: 'values',
  inh: false,
  tq: '',
  lastTid: -1
};

function chipOf(m){
  return m.cat === 'bool' ? 'bool' : m.cat === 'num' ? 'num' : m.cat === 'method' ? 'method' : 'other';
}

function searching(){
  return S.q.trim() !== '' || S.ns !== null || S.pub || S.stat || S.cst || S.asm !== -1 ||
    !(S.chips.bool && S.chips.num && S.chips.other && S.chips.method);
}

/* ---------------- SEARCH ENGINE ---------------- */
function search(){
  var toks = S.q.split(/\s+/).map(norm).filter(Boolean), joined = toks.join('');
  var counts = { bool: 0, num: 0, other: 0, method: 0 }, groups = new Map(), total = 0;

  T.forEach(function(tp){
    if(S.asm !== -1 && tp.a !== S.asm) return;
    if(S.ns !== null && tp.ns !== S.ns) return;
    var tk = tp._k;
    for(var j = 0; j < tp.mem.length; j++){
      var m = tp.mem[j];
      if(!S.cst && m.c !== undefined) continue;
      if(S.pub && !isPub(m.a)) continue;
      if(S.stat && !m.s) continue;
      var sc = 3;
      if(toks.length){
        var any = false, ok = true;
        for(var t = 0; t < toks.length; t++){
          if(m.k.indexOf(toks[t]) >= 0) any = true;
          else if(tk.indexOf(toks[t]) < 0){ ok = false; break; }
        }
        if(!ok || !any) continue;
        sc = m.k === joined ? 0 : (m.k.indexOf(toks[0]) === 0 ? 1 : 2);
      }
      var c = chipOf(m);
      counts[c]++;
      if(!S.chips[c]) continue;
      var g = groups.get(tp);
      if(!g){ g = { tp: tp, rows: [], best: 9 }; groups.set(tp, g); }
      m._s = sc;
      g.rows.push(m);
      if(sc < g.best) g.best = sc;
      total++;
    }
  });

  var arr = Array.from(groups.values());
  arr.forEach(function(g){
    g.rows.sort(function(a, b){ return a._s - b._s || ORDER[a.cat] - ORDER[b.cat] || a.n.localeCompare(b.n); });
  });
  arr.sort(function(a, b){ return a.best - b.best || a.tp.n.localeCompare(b.tp.n); });

  var tmatch = [];
  if(toks.length){
    T.forEach(function(tp){
      if(S.asm !== -1 && tp.a !== S.asm) return;
      if(S.ns !== null && tp.ns !== S.ns) return;
      for(var t = 0; t < toks.length; t++) if(tp._k.indexOf(toks[t]) < 0) return;
      tmatch.push(tp);
    });
    tmatch.sort(function(a, b){ return (a._k === joined ? 0 : 1) - (b._k === joined ? 0 : 1) || a.n.length - b.n.length; });
  }

  return { groups: arr, counts: counts, total: total, tmatch: tmatch };
}

function badge(m){
  switch(m.cat){
    case 'bool': return ['BOOL', 'b-bool'];
    case 'num': return ['NUM', 'b-num'];
    case 'text': return ['TEXT', 'b-text'];
    case 'enum': return ['ENUM', 'b-enum'];
    case 'method': return ['FUNC', 'b-func'];
    case 'event': return ['EVENT', 'b-evt'];
    default: return ['OBJ', 'b-obj'];
  }
}

function tags(m){
  var h = '<span class="tg ' + (isPub(m.a) ? 'pub' : '') + '">[' + ACC[m.a].toUpperCase() + ']</span>';
  if(m.s) h += '<span class="tg st">[STATIC]</span>';
  if(m.ro) h += '<span class="tg ro">[RO]</span>';
  if(m.kind === 'prop' && m.w === undefined) h += '<span class="tg ro">[GET_ONLY]</span>';
  if(m.mod) h += '<span class="tg">[' + esc(m.mod).toUpperCase() + ']</span>';
  if(m.at) m.at.slice(0, 3).forEach(function(a){ h += '<span class="tg at">[' + esc(a) + ']</span>'; });
  return h;
}

function rowHTML(m, showOwner){
  var b = badge(m), dim = (m.kind === 'prop' && m.w === undefined) ? ' dim' : '', name, sub;
  if(m.kind === 'method'){
    name = '<b>' + esc(m.n) + (m.gp ? '&lt;' + esc(m.gp) + '&gt;' : '') + '</b>';
    sub = '<div class="sig">' + esc(short(m.r)) + ' ' + esc(m.n) + '(' + m.p.map(function(p){ return esc(short(p[0]) + ' ' + p[1]); }).join(', ') + ')</div>';
  } else {
    name = '<b>' + esc(hum(m.n)) + '</b><code class="raw">' + esc(m.n) + '</code>';
    var tv = short(m.ty);
    if(m.ix && m.ix.length) tv += ' [' + m.ix.map(short).join(', ') + ']';
    if(m.c !== undefined && m.c !== '') tv += ' = ' + m.c;
    sub = '<div class="sig">' + esc(tv) + '</div>';
  }
  if(showOwner) name += '<span class="own">[' + esc(m.o.n) + ']</span>';

  return '<div class="row' + dim + '"><span class="badge">' + b[0] + '</span><div class="nm">' + name + sub + '</div>' +
    '<div class="tgs">' + tags(m) + '</div><div class="act">' +
    '<button data-act="name" data-t="' + m.o.id + '" data-i="' + m.i + '">' + tr('cName') + '</button>' +
    (m.kind === 'event' ? '' : '<button data-act="' + (m.kind === 'method' ? 'harm' : 'cs') + '" data-t="' + m.o.id + '" data-i="' + m.i + '">' + (m.kind === 'method' ? tr('cH') : tr('cCs')) + '</button>') +
    '</div></div>';
}

function kindBadge(tp){ return '<span class="kind">[' + tp.k.toUpperCase() + ']</span>'; }
function nsPart(tp){ return tp.ns ? '<span class="ns">' + esc(tp.ns) + '.</span>' : ''; }
function sortByName(a, b){ return a.n.localeCompare(b.n); }

var STOP = {};
('get set is has can the and for new old use should to from on off of in out are all this that init cur prev tmp temp val var obj ref idx index id name type data list num count value flag enabled enable current max min default state mode size length total'.split(' ')).forEach(function(w){ STOP[w] = 1; });

function topWords(){
  var cnt = {};
  T.forEach(function(tp){
    tp.mem.forEach(function(m){
      if(m.c !== undefined || m.cat === 'method' || m.cat === 'event' || m.cat === 'other') return;
      var seen = {};
      String(m.n).replace(/^(m_|s_|_+)/, '').replace(/([a-z0-9])([A-Z])/g, '$1 $2').replace(/([A-Z]+)([A-Z][a-z])/g, '$1 $2').split(/[^A-Za-z]+/).forEach(function(w){
        w = w.toLowerCase();
        if(w.length < 3 || STOP[w] || seen[w]) return;
        seen[w] = 1; cnt[w] = (cnt[w] || 0) + 1;
      });
    });
  });
  return Object.keys(cnt).filter(function(w){ return cnt[w] >= 3; }).sort(function(a, b){ return cnt[b] - cnt[a] || a.localeCompare(b); }).slice(0, 18).map(function(w){ return [w, cnt[w]]; });
}

function updateChips(counts){
  var defs = [['bool', '[FLAGS]'], ['num', '[STATS]'], ['other', '[OBJECTS]'], ['method', '[FUNCS]']];
  var h = '';
  defs.forEach(function(d){
    var n = counts ? counts[d[0]] : '';
    h += '<div class="chip' + (S.chips[d[0]] ? '' : ' off') + '" data-chip="' + d[0] + '"><i></i>' + d[1] + '<span class="n">' + n + '</span></div>';
  });
  $('#chips').innerHTML = h;
}

/* ---------------- CODE GENERATION SNIPPETS ---------------- */
function chainOf(tp){
  var out = [], cur = tp, g = 0;
  while(cur && cur.b && g++ < 24){
    var bt = byFull.get(strip(cur.b));
    out.push({ name: cur.b, t: bt });
    cur = bt;
  }
  return out;
}

function isUnity(tp){
  return chainOf(tp).some(function(c){ return /MonoBehaviour|ScriptableObject/.test(c.name); });
}

function tfull(tp){ return (tp.ns ? tp.ns + '.' : '') + tp.n; }
function rtname(tp){
  var n = (tp.ns ? tp.ns + '.' : '') + tp.n.replace(/\./g, '+');
  if(tp.g) n += '`' + (tp.g.split(',').length);
  return n;
}
function tof(tp){
  var n = tfull(tp);
  if(tp.g) n += '<' + new Array(tp.g.split(',').length).join(',') + '>';
  return n;
}
function typeAccessible(tp){ return tp.v === 1 || tp.v === 2; }
function litFor(m){
  var t = m.ty;
  if(m.cat === 'bool') return 'true';
  if(t === 'float') return '999f';
  if(t === 'double') return '999d';
  if(m.cat === 'num') return '999';
  if(m.cat === 'text') return '"value"';
  return 'default';
}

function csSnippet(m){
  var tp = m.o, val = litFor(m), inst = m.s ? 'null' : 'instance', T0 = tfull(tp);
  if(m.kind === 'event') return '// ' + T0 + '.' + m.n;
  var direct = m.a === 6 && typeAccessible(tp) && !(m.kind === 'prop' && m.w === undefined) && !(m.kind === 'field' && m.ro);
  var s = '';
  if(!m.s) s += '// instance of ' + T0 + ' - see "' + tr('howGet') + '"\n';
  if(direct){
    s += (m.s ? '' : T0 + ' instance = ...;\n') + (m.s ? T0 : 'instance') + '.' + m.n + ' = ' + val + ';';
  } else {
    var tyx = typeAccessible(tp) ? 'typeof(' + tof(tp) + ')' : 'AccessTools.TypeByName("' + rtname(tp) + '")';
    s += 'using HarmonyLib;\n' + (m.s ? '' : T0 + ' instance = ...;\n');
    s += 'AccessTools.' + (m.kind === 'prop' ? 'Property' : 'Field') + '(' + tyx + ', "' + m.n + '").SetValue(' + inst + ', ' + val + ');';
  }
  return s;
}

function harmonySnippet(m){
  var tp = m.o, cn = tp.n.replace(/[^\w]/g, '_');
  var ps = m.p.map(function(p){ return '//   ' + p[0] + ' ' + p[1]; }).join('\n');
  var s = 'using HarmonyLib;\n\n[HarmonyPatch("' + rtname(tp) + '", "' + m.n + '")]\nclass Patch_' + cn + '_' + m.n.replace(/[^\w]/g, '_') + '\n{\n' +
    '    // Prefix: return false to skip original\n    static bool Prefix(' + (m.s ? '' : 'object __instance') + ')\n    {\n        return true;\n    }\n\n' +
    '    // Postfix: runs after; modify result via "ref ' + (m.r === 'void' ? '...' : short(m.r)) + ' __result"\n    static void Postfix(' + (m.s ? '' : 'object __instance') + ')\n    {\n    }\n}';
  if(ps) s += '\n// original parameters:\n' + ps;
  return s;
}

function holders(tp){
  var list = staticBy.get(tp.f) || [];
  return list.filter(function(h){ return h.m.a !== 0; });
}

/* ===================================================================
   RIGID-BODY ELASTIC COLLISION NODE GRAPH ENGINE (STRICT MONOCHROME)
   NO IDLE DRIFT • REAL BALL COLLISIONS • SEQUENTIAL BURST EMERGENCE
   =================================================================== */

var G_STATE = {
  level: 0,
  path: [{ id: 'root', label: 'ROOT_KERNEL' }],
  nodes: [],
  links: [],
  emergenceQueue: [],
  lastEjectTime: 0,
  targetNode: null,
  animState: 'idle',    // 'idle' | 'imploding' | 'ejecting'
  animStart: 0,
  animDur: 360,
  hovered: null,
  dragged: null,
  lastMouseX: 0,
  lastMouseY: 0,
  mouseVx: 0,
  mouseVy: 0,
  canvas: null,
  ctx: null,
  w: 800,
  h: 560,
  dpr: 1,
  rafId: null,
  isTall: false
};

// Build nodes hierarchy (more nodes: 20-30 per view!)
function buildGraphData(level, currentEntity){
  var nodes = [], links = [];
  var cx = G_STATE.w / 2, cy = G_STATE.h / 2;

  if(level === 0){
    // LEVEL 0: Main Hub + Top 18 Namespaces + 4 Category Hubs
    var rootNode = {
      id: 'root_hub',
      label: (D.game || 'MONO_KERNEL').toUpperCase(),
      sub: totals.t + ' TYPES',
      type: 'root',
      r: 38,
      mass: 8,
      x: cx, y: cy,
      vx: 0, vy: 0,
      active: true,
      isParent: true,
      expandable: false,
      data: null
    };
    nodes.push(rootNode);

    var topNs = NSLIST.slice().sort(function(a, b){ return (nsCount[b] || 0) - (nsCount[a] || 0); }).slice(0, 18);
    topNs.forEach(function(nsName, idx){
      var n = {
        id: 'ns_' + nsName,
        label: (nsName || 'GLOBAL').toUpperCase(),
        sub: nsCount[nsName] + ' TYPES',
        type: 'ns',
        r: 22 + Math.min(10, Math.sqrt(nsCount[nsName] || 1) * 1.5),
        mass: 2,
        x: cx, y: cy,
        vx: 0, vy: 0,
        active: false,
        expandable: true,
        data: { ns: nsName }
      };
      nodes.push(n);
      links.push({ source: rootNode, target: n });
    });

    // 4 Category Hubs
    var hubs = [
      { id: 'hub_bools', label: tr('hubBools'), sub: totals.b + ' FLAGS', kind: 'bool' },
      { id: 'hub_nums', label: tr('hubNums'), sub: totals.n + ' STATS', kind: 'num' },
      { id: 'hub_methods', label: tr('hubMethods'), sub: totals.f + ' HOOKS', kind: 'method' },
      { id: 'hub_statics', label: tr('hubStatics'), sub: 'GLOBAL', kind: 'static' }
    ];

    hubs.forEach(function(hb){
      var hn = {
        id: hb.id,
        label: hb.label,
        sub: hb.sub,
        type: 'hub',
        r: 26,
        mass: 3,
        x: cx, y: cy,
        vx: 0, vy: 0,
        active: false,
        expandable: true,
        data: { hub: hb.kind }
      };
      nodes.push(hn);
      links.push({ source: rootNode, target: hn });
    });

  } else if(level === 1){
    // LEVEL 1: Inside Namespace or Hub (up to 26 classes!)
    var parent = currentEntity;
    var centerNode = {
      id: 'center_p',
      label: parent.label,
      sub: parent.sub,
      type: parent.type,
      r: 36,
      mass: 8,
      x: cx, y: cy,
      vx: 0, vy: 0,
      active: true,
      isParent: true,
      expandable: false,
      data: parent.data
    };
    nodes.push(centerNode);

    var childClasses = [];
    if(parent.type === 'ns'){
      var targetNs = parent.data.ns;
      childClasses = T.filter(function(tp){ return tp.ns === targetNs; })
        .sort(function(a, b){ return (b.mem ? b.mem.length : 0) - (a.mem ? a.mem.length : 0); })
        .slice(0, 26);
    } else if(parent.type === 'hub'){
      var hk = parent.data.hub;
      var withCat = [];
      T.forEach(function(tp){
        var cnt = 0;
        tp.mem.forEach(function(m){
          if(hk === 'static' ? m.s : m.cat === hk) cnt++;
        });
        if(cnt > 0) withCat.push({ tp: tp, cnt: cnt });
      });
      withCat.sort(function(a, b){ return b.cnt - a.cnt; });
      childClasses = withCat.slice(0, 26).map(function(x){ return x.tp; });
    }

    childClasses.forEach(function(tp){
      var cn = {
        id: 'cls_' + tp.id,
        label: tp.n.toUpperCase(),
        sub: (tp.mem ? tp.mem.length : 0) + ' MEMBERS',
        type: 'class',
        r: 20 + Math.min(8, (tp.mem ? tp.mem.length : 0) / 12),
        mass: 2,
        x: cx, y: cy,
        vx: 0, vy: 0,
        active: false,
        expandable: true,
        data: tp
      };
      nodes.push(cn);
      links.push({ source: centerNode, target: cn });
    });

  } else if(level === 2){
    // LEVEL 2: Inside a Class (up to 24 key members!)
    var tp = currentEntity.data;
    var classCenter = {
      id: 'center_cls',
      label: tp.n.toUpperCase(),
      sub: (tp.ns ? tp.ns + ' • ' : '') + tp.k.toUpperCase(),
      type: 'class',
      r: 36,
      mass: 8,
      x: cx, y: cy,
      vx: 0, vy: 0,
      active: true,
      isParent: true,
      expandable: false,
      data: tp
    };
    nodes.push(classCenter);

    var mems = tp.mem || [];
    var bools = mems.filter(function(m){ return m.cat === 'bool' && m.c === undefined; }).slice(0, 8);
    var nums = mems.filter(function(m){ return m.cat === 'num' && m.c === undefined; }).slice(0, 8);
    var methods = mems.filter(function(m){ return m.kind === 'method'; }).slice(0, 8);
    var displayedMems = bools.concat(nums).concat(methods);

    displayedMems.forEach(function(m){
      var mn = {
        id: 'mem_' + m.i,
        label: m.n.toUpperCase(),
        sub: short(m.ty || m.r || ''),
        type: 'member',
        cat: m.cat,
        r: 18,
        mass: 1.5,
        x: cx, y: cy,
        vx: 0, vy: 0,
        active: false,
        expandable: false,
        data: m
      };
      nodes.push(mn);
      links.push({ source: classCenter, target: mn });
    });

    // Inspector Shortcut Node
    var actNode = {
      id: 'act_inspect',
      label: tr('inspectClass'),
      sub: 'OPEN_FULL_INSPECTOR',
      type: 'action',
      r: 22,
      mass: 2,
      x: cx, y: cy,
      vx: 0, vy: 0,
      active: false,
      expandable: false,
      data: tp
    };
    nodes.push(actNode);
    links.push({ source: classCenter, target: actNode });
  }

  G_STATE.nodes = nodes;
  G_STATE.links = links;

  // Prepare Sequential Emergence Queue
  G_STATE.emergenceQueue = nodes.filter(function(n){ return !n.isParent; });
  G_STATE.lastEjectTime = performance.now();
  G_STATE.animState = 'ejecting';
}

// Trigger "Всасывание" (Implosion)
function triggerSuckAndUnfold(clickedNode){
  if(!clickedNode.expandable) return;

  G_STATE.targetNode = clickedNode;
  G_STATE.animState = 'imploding';
  G_STATE.animStart = performance.now();

  G_STATE.nodes.forEach(function(n){
    n.startX = n.x;
    n.startY = n.y;
  });
}

// Finish Implosion and Start Sequential Emergence
function finishImplosionAndUnfold(){
  var clicked = G_STATE.targetNode;
  G_STATE.level++;
  G_STATE.path.push({
    id: clicked.id,
    label: clicked.label,
    entity: clicked
  });
  updateBreadcrumbsHUD();

  buildGraphData(G_STATE.level, clicked);
  G_STATE.targetNode = null;
}

function zoomOutOneLevel(){
  if(G_STATE.level <= 0) return;
  G_STATE.level--;
  G_STATE.path.pop();
  updateBreadcrumbsHUD();

  var prevEntity = G_STATE.path[G_STATE.path.length - 1];
  buildGraphData(G_STATE.level, prevEntity ? prevEntity.entity : null);
}

function updateBreadcrumbsHUD(){
  var el = $('#graphBreadcrumbs');
  if(!el) return;
  var html = '';
  G_STATE.path.forEach(function(p, i){
    if(i > 0) html += '<span class="crumb-sep">/</span>';
    var isLast = i === G_STATE.path.length - 1;
    html += '<button class="crumb-btn' + (isLast ? ' active' : '') + '" data-crumb="' + i + '">' + esc(p.label) + '</button>';
  });
  el.innerHTML = html;

  var backBtn = $('#graphBackBtn');
  if(backBtn) backBtn.style.display = G_STATE.level > 0 ? '' : 'none';
}

/* ---------------- RIGID BODY ELASTIC COLLISION PHYSICS ---------------- */
function resolveCollisionsAndPhysics(){
  var nodes = G_STATE.nodes;
  var len = nodes.length;
  var W = G_STATE.w, H = G_STATE.h;
  var restitution = 0.84; // Elastic bounce

  // 1. Circle-to-Circle Elastic Collision
  for(var i = 0; i < len; i++){
    var a = nodes[i];
    if(!a.active) continue;

    for(var j = i + 1; j < len; j++){
      var b = nodes[j];
      if(!b.active) continue;

      var dx = b.x - a.x;
      var dy = b.y - a.y;
      var distSq = dx * dx + dy * dy;
      var minDist = a.r + b.r;

      if(distSq < minDist * minDist && distSq > 0.0001){
        var dist = Math.sqrt(distSq);
        var nx = dx / dist;
        var ny = dy / dist;

        // Position overlap separation
        var overlap = minDist - dist;
        var totalMass = a.mass + b.mass;
        var ratioA = a.isParent ? 0 : (b.isParent ? 1 : b.mass / totalMass);
        var ratioB = b.isParent ? 0 : (a.isParent ? 1 : a.mass / totalMass);

        if(a !== G_STATE.dragged){
          a.x -= nx * overlap * ratioA;
          a.y -= ny * overlap * ratioA;
        }
        if(b !== G_STATE.dragged){
          b.x += nx * overlap * ratioB;
          b.y += ny * overlap * ratioB;
        }

        // Relative velocity exchange along collision normal
        var rvx = b.vx - a.vx;
        var rvy = b.vy - a.vy;
        var velAlongNormal = rvx * nx + rvy * ny;

        if(velAlongNormal < 0){ // Moving towards each other
          var impulse = -(1 + restitution) * velAlongNormal / (1 / a.mass + 1 / b.mass);
          var ix = impulse * nx;
          var iy = impulse * ny;

          if(!a.isParent && a !== G_STATE.dragged){
            a.vx -= ix / a.mass;
            a.vy -= iy / a.mass;
          }
          if(!b.isParent && b !== G_STATE.dragged){
            b.vx += ix / b.mass;
            b.vy += iy / b.mass;
          }
        }
      }
    }
  }

  // 2. Integration & Wall Boundary Collisions
  var margin = 14;
  for(var k = 0; k < len; k++){
    var n = nodes[k];
    if(!n.active || n.isParent) continue;

    if(n !== G_STATE.dragged){
      n.x += n.vx;
      n.y += n.vy;

      // Friction / air damping
      n.vx *= 0.95;
      n.vy *= 0.95;

      // STRICT IDLE: Dead stop when velocity is negligible (NO floating in idle!)
      if(Math.abs(n.vx) < 0.03) n.vx = 0;
      if(Math.abs(n.vy) < 0.03) n.vy = 0;

      // Wall bounce
      if(n.x - n.r < margin){ n.x = margin + n.r; n.vx = -n.vx * 0.75; }
      if(n.x + n.r > W - margin){ n.x = W - margin - n.r; n.vx = -n.vx * 0.75; }
      if(n.y - n.r < margin){ n.y = margin + n.r; n.vy = -n.vy * 0.75; }
      if(n.y + n.r > H - margin){ n.y = H - margin - n.r; n.vy = -n.vy * 0.75; }
    }
  }
}

// Canvas Render & Physics Loop
function runGraphLoop(){
  if(!G_STATE.ctx || !document.getElementById('graphCanvas')){
    G_STATE.rafId = null;
    return;
  }
  var ctx = G_STATE.ctx;
  var now = performance.now();
  var cx = G_STATE.w / 2, cy = G_STATE.h / 2;

  ctx.clearRect(0, 0, G_STATE.w, G_STATE.h);

  // Crosshair in center
  ctx.save();
  ctx.strokeStyle = '#222222';
  ctx.lineWidth = 1;
  ctx.beginPath();
  ctx.moveTo(cx - 30, cy); ctx.lineTo(cx + 30, cy);
  ctx.moveTo(cx, cy - 30); ctx.lineTo(cx, cy + 30);
  ctx.stroke();
  ctx.restore();

  // 1. SEQUENTIAL BURST EMERGENCE ("быстро вылазить по очереди из главного")
  if(G_STATE.animState === 'ejecting'){
    if(G_STATE.emergenceQueue.length > 0){
      // Eject one node every 28 milliseconds
      if(now - G_STATE.lastEjectTime > 28){
        var nextNode = G_STATE.emergenceQueue.shift();
        nextNode.active = true;
        nextNode.x = cx;
        nextNode.y = cy;

        // Radial burst direction with high kinetic impulse
        var total = G_STATE.nodes.length - 1;
        var idx = total - G_STATE.emergenceQueue.length;
        var angle = (idx / (total || 1)) * Math.PI * 2 + (Math.random() - 0.5) * 0.4;
        var ejectSpeed = 9 + Math.random() * 5;

        nextNode.vx = Math.cos(angle) * ejectSpeed;
        nextNode.vy = Math.sin(angle) * ejectSpeed;

        G_STATE.lastEjectTime = now;
      }
    } else {
      G_STATE.animState = 'idle';
    }
  }

  // 2. IMPLOSION VORTEX ("всасывание")
  if(G_STATE.animState === 'imploding'){
    var elapsed = now - G_STATE.animStart;
    var progress = Math.min(1, elapsed / G_STATE.animDur);
    var ease = progress * progress * progress;
    var target = G_STATE.targetNode;

    target.x += (cx - target.x) * 0.15;
    target.y += (cy - target.y) * 0.15;

    // Draw singularity white-hot pulse
    ctx.save();
    ctx.strokeStyle = '#ffffff';
    ctx.lineWidth = 2;
    ctx.globalAlpha = 0.8 * (1 - progress);
    ctx.beginPath();
    ctx.arc(target.x, target.y, target.r * (1 + progress * 3), 0, Math.PI * 2);
    ctx.stroke();

    // Gravitational suction lines
    for(var s = 0; s < 16; s++){
      var sAng = (s * (Math.PI * 2 / 16)) + (progress * 6);
      var sDist = (1 - ease) * 220 + 8;
      var sx = target.x + Math.cos(sAng) * sDist;
      var sy = target.y + Math.sin(sAng) * sDist;
      ctx.beginPath();
      ctx.moveTo(sx, sy);
      ctx.lineTo(target.x, target.y);
      ctx.stroke();
    }
    ctx.restore();

    G_STATE.nodes.forEach(function(n){
      if(n !== target){
        n.x = n.startX + (target.x - n.startX) * ease;
        n.y = n.startY + (target.y - n.startY) * ease;
        n.scale = Math.max(0, 1 - ease);
      } else {
        n.scale = 1 + Math.sin(progress * Math.PI) * 0.4;
      }
    });

    if(progress >= 1){
      finishImplosionAndUnfold();
    }
  } else {
    // Run Rigid-Body Collision Resolution
    resolveCollisionsAndPhysics();
  }

  // 3. Draw Synaptic Links
  G_STATE.links.forEach(function(lnk){
    var s = lnk.source, t = lnk.target;
    if(!s.active || !t.active) return;
    var isLit = (G_STATE.hovered === s || G_STATE.hovered === t);

    ctx.save();
    ctx.strokeStyle = isLit ? '#ffffff' : 'rgba(255, 255, 255, 0.15)';
    ctx.lineWidth = isLit ? 1.8 : 0.8;
    ctx.beginPath();
    ctx.moveTo(s.x, s.y);
    ctx.lineTo(t.x, t.y);
    ctx.stroke();
    ctx.restore();
  });

  // 4. Draw Rigid Spheres / Nodes
  G_STATE.nodes.forEach(function(n){
    if(!n.active) return;
    var scale = n.scale !== undefined ? n.scale : 1;
    if(scale <= 0.001) return;
    var r = n.r * scale;
    var isHover = (G_STATE.hovered === n);

    ctx.save();

    // Outer Glow on Hover or Parent
    if(isHover || n.isParent){
      ctx.strokeStyle = '#ffffff';
      ctx.lineWidth = 1;
      ctx.globalAlpha = isHover ? 0.4 : 0.2;
      ctx.beginPath();
      ctx.arc(n.x, n.y, r + 6, 0, Math.PI * 2);
      ctx.stroke();
    }

    // Node Body: Pure Black with White Rim
    ctx.globalAlpha = 1;
    ctx.fillStyle = n.isParent ? '#ffffff' : '#000000';
    ctx.beginPath();
    ctx.arc(n.x, n.y, r, 0, Math.PI * 2);
    ctx.fill();

    // Rim
    ctx.strokeStyle = '#ffffff';
    ctx.lineWidth = isHover ? 2.2 : (n.isParent ? 3 : 1.2);
    ctx.stroke();

    // Center Core Indicator
    ctx.fillStyle = n.isParent ? '#000000' : '#ffffff';
    ctx.beginPath();
    ctx.arc(n.x, n.y, n.isParent ? 5 : 2.5, 0, Math.PI * 2);
    ctx.fill();

    // Label underneath
    if(scale > 0.4){
      ctx.textAlign = 'center';
      ctx.textBaseline = 'top';

      var labelText = n.label;
      if(labelText.length > 18) labelText = labelText.slice(0, 17) + '…';

      ctx.font = '700 10px monospace';
      var textW = ctx.measureText(labelText).width;
      var pillW = textW + 10, pillH = 16;
      var pillX = n.x - pillW / 2, pillY = n.y + r + 4;

      ctx.fillStyle = '#000000';
      ctx.strokeStyle = isHover ? '#ffffff' : '#333333';
      ctx.lineWidth = 1;
      ctx.beginPath();
      if(ctx.roundRect) ctx.roundRect(pillX, pillY, pillW, pillH, 2);
      else ctx.rect(pillX, pillY, pillW, pillH);
      ctx.fill();
      ctx.stroke();

      ctx.fillStyle = isHover ? '#ffffff' : '#b5b5b5';
      ctx.fillText(labelText, n.x, pillY + 2.5);

      if(n.sub && isHover){
        ctx.font = '9px monospace';
        ctx.fillStyle = '#ffffff';
        ctx.fillText(n.sub, n.x, pillY + 19);
      }
    }

    ctx.restore();
  });

  G_STATE.rafId = requestAnimationFrame(runGraphLoop);
}

// Setup Canvas DOM & Event Listeners
function initGraphCanvas(){
  var canvas = $('#graphCanvas');
  if(!canvas) return;

  if(G_STATE.rafId){
    cancelAnimationFrame(G_STATE.rafId);
    G_STATE.rafId = null;
  }

  G_STATE.canvas = canvas;
  G_STATE.ctx = canvas.getContext('2d');
  G_STATE.dpr = window.devicePixelRatio || 1;

  function resize(){
    var rect = canvas.getBoundingClientRect();
    G_STATE.w = rect.width || 800;
    G_STATE.h = rect.height || 560;
    canvas.width = G_STATE.w * G_STATE.dpr;
    canvas.height = G_STATE.h * G_STATE.dpr;
    G_STATE.ctx.scale(G_STATE.dpr, G_STATE.dpr);

    if(!G_STATE.nodes.length){
      buildGraphData(G_STATE.level, null);
    }
  }

  resize();
  window.addEventListener('resize', resize);

  // Mouse Drag with Real Kinetic Momentum / Throw Physics
  canvas.addEventListener('mousemove', function(e){
    var rect = canvas.getBoundingClientRect();
    var mx = e.clientX - rect.left;
    var my = e.clientY - rect.top;

    G_STATE.mouseVx = (mx - G_STATE.lastMouseX) * 0.7;
    G_STATE.mouseVy = (my - G_STATE.lastMouseY) * 0.7;
    G_STATE.lastMouseX = mx;
    G_STATE.lastMouseY = my;

    if(G_STATE.dragged){
      G_STATE.dragged.x = mx;
      G_STATE.dragged.y = my;
      G_STATE.dragged.vx = G_STATE.mouseVx;
      G_STATE.dragged.vy = G_STATE.mouseVy;
      return;
    }

    var hit = null;
    for(var i = G_STATE.nodes.length - 1; i >= 0; i--){
      var n = G_STATE.nodes[i];
      if(!n.active) continue;
      var dx = mx - n.x, dy = my - n.y;
      if(dx * dx + dy * dy <= (n.r + 6) * (n.r + 6)){
        hit = n;
        break;
      }
    }
    G_STATE.hovered = hit;
    canvas.style.cursor = hit ? 'pointer' : 'default';
    updateTooltip(hit, e.clientX, e.clientY);
  });

  canvas.addEventListener('mousedown', function(e){
    if(e.button !== 0) return;
    if(G_STATE.hovered){
      G_STATE.dragged = G_STATE.hovered;
      G_STATE.dragged.vx = 0;
      G_STATE.dragged.vy = 0;
    }
  });

  window.addEventListener('mouseup', function(){
    if(G_STATE.dragged){
      // Give throw velocity to node on release
      G_STATE.dragged.vx = G_STATE.mouseVx;
      G_STATE.dragged.vy = G_STATE.mouseVy;
      G_STATE.dragged = null;
    }
  });

  // Double Click Event: Implode and Burst
  canvas.addEventListener('dblclick', function(e){
    e.preventDefault();
    if(G_STATE.hovered){
      var n = G_STATE.hovered;
      if(n.expandable){
        triggerSuckAndUnfold(n);
      } else if(n.type === 'action'){
        location.hash = '#t/' + n.data.id;
      } else if(n.type === 'member'){
        var m = n.data;
        var code = (m.kind === 'method') ? harmonySnippet(m) : csSnippet(m);
        copy(code);
        toast(tr('copied') + ':\n' + m.n);
      } else if(n.isParent){
        zoomOutOneLevel();
      }
    } else {
      zoomOutOneLevel();
    }
  });

  runGraphLoop();
}

function updateTooltip(node, clientX, clientY){
  var tt = $('#graphTooltip');
  if(!tt) return;
  if(!node){
    tt.style.opacity = '0';
    return;
  }

  var actionText = node.expandable ? '[DBL-CLICK: EXPAND]' : (node.type === 'member' ? '[DBL-CLICK: COPY CODE]' : (node.type === 'action' ? '[DBL-CLICK: OPEN INSPECTOR]' : ''));
  tt.innerHTML = '<div class="gt-title">:: ' + esc(node.label) + '</div>' +
    '<div class="gt-sub">' + esc(node.sub || '') + '</div>' +
    (actionText ? '<div class="gt-action">' + actionText + '</div>' : '');

  var parentRect = tt.parentElement.getBoundingClientRect();
  tt.style.left = (clientX - parentRect.left) + 'px';
  tt.style.top = (clientY - parentRect.top) + 'px';
  tt.style.opacity = '1';
}

/* ---------------- HTML VIEWS ---------------- */

// TAB 1: Neural Graph Centerpiece + Overview
function graphViewHTML(){
  var h = '<div class="graph-section" id="graphSection">' +
    '<div class="graph-hud-top">' +
      '<div class="graph-breadcrumbs" id="graphBreadcrumbs"></div>' +
      '<button class="level-back-btn" id="graphBackBtn" style="display:none">' + tr('levelUp') + '</button>' +
      '<div class="graph-controls">' +
        '<button class="ctrl-btn" id="resetGraphBtn">[RESET_GRID]</button>' +
        '<button class="ctrl-btn" id="expandGraphBtn">[HEIGHT_TOGGLE]</button>' +
      '</div>' +
    '</div>' +
    '<canvas class="graph-canvas" id="graphCanvas"></canvas>' +
    '<div class="graph-tooltip" id="graphTooltip" style="opacity:0"></div>' +
    '<div class="graph-hud-bottom">' +
      '<span class="graph-hint">' + tr('graphHint') + '</span>' +
    '</div>' +
  '</div>';

  h += '<div class="tiles">' +
    '<div class="tile"><b>' + totals.t + '</b><span>' + tr('totalT') + '</span></div>' +
    '<div class="tile click" data-quick="bool"><b>' + totals.b + '</b><span>' + tr('allBool') + '</span></div>' +
    '<div class="tile click" data-quick="num"><b>' + totals.n + '</b><span>' + tr('allNum') + '</span></div>' +
    '<div class="tile click" data-quick="method"><b>' + totals.f + '</b><span>' + tr('allFn') + '</span></div>' +
  '</div>';

  var sg = '';
  topWords().forEach(function(w){
    sg += '<span class="sg" data-q="' + esc(w[0]) + '">' + esc(w[0]).toUpperCase() + '<small>' + w[1] + '</small></span>';
  });
  if(sg) h += '<h2>// ' + tr('tryIt') + '</h2><div class="sugg">' + sg + '</div>';

  return h;
}

// TAB 2: Directory Index / Search
function resultsHTML(){
  var r = search();
  updateChips(r.counts);

  var onlyNs = S.q.trim() === '' && S.ns !== null && S.chips.bool && S.chips.num && S.chips.other && S.chips.method && !S.pub && !S.stat;
  if(onlyNs){
    var list = T.filter(function(tp){ return tp.ns === S.ns && (S.asm === -1 || tp.a === S.asm); }).sort(sortByName);
    var h = '<div class="info">' + list.length + ' ' + tr('types') + '</div><div class="tcards">';
    list.forEach(function(tp){
      h += '<div class="tcard" data-open="' + tp.id + '">' + kindBadge(tp) + ' <b>' + esc(tp.n) + '</b><small>' + tp.mem.length + ' ' + tr('members') + '</small></div>';
    });
    return h + '</div>';
  }

  var h2 = '';
  if(r.tmatch.length){
    h2 += '<h2>// ' + tr('classes') + ' (' + r.tmatch.length + ')</h2><div class="tcards">';
    r.tmatch.slice(0, 16).forEach(function(tp){
      h2 += '<div class="tcard" data-open="' + tp.id + '">' + kindBadge(tp) + ' <b>' + nsPart(tp) + esc(tp.n) + '</b><small>' + tp.mem.length + ' ' + tr('members') + '</small></div>';
    });
    h2 += '</div>';
  }

  if(!r.total && !r.tmatch.length) return '<div class="empty">' + tr('none') + '</div>';

  if(r.total){
    h2 += '<h2>// ' + tr('found') + ' ' + r.total + ' ' + tr('inCls') + ' ' + r.groups.length + '</h2>';
    r.groups.slice(0, S.limit).forEach(function(g){
      var tp = g.tp, cap = S.exp[tp.id] ? 9999 : 25;
      h2 += '<section class="grp"><div class="gh">' + kindBadge(tp) + '<span class="cn" data-open="' + tp.id + '">' + nsPart(tp) + esc(tp.n) + '</span><span class="cnt">COUNT:: ' + g.rows.length + '</span></div>';
      g.rows.slice(0, cap).forEach(function(m){ h2 += rowHTML(m, false); });
      if(g.rows.length > cap) h2 += '<button class="morebtn" data-exp="' + tp.id + '">' + tr('rowsMore') + ' (' + (g.rows.length - cap) + ')</button>';
      h2 += '</section>';
    });
    if(r.groups.length > S.limit) h2 += '<button class="bigmore" data-more="1">' + tr('more') + ' (' + (r.groups.length - S.limit) + ')</button>';
  }
  return h2;
}

// TAB 3: Exploit Hooks
function cheatHooksHTML(){
  var h = '<div class="cheat-hub-header">' +
    '<h2>// ' + tr('cheatHubTitle') + '</h2>' +
    '<p>' + tr('cheatHubSub') + '</p>' +
  '</div>';

  var cheatKeywords = ['god', 'immortal', 'invulnerable', 'health', 'hp', 'speed', 'ammo', 'gold', 'money', 'coin', 'damage', 'kill', 'fly', 'noclip', 'dead', 'die', 'mana', 'score', 'xp', 'level', 'stamina'];
  var highValueVars = [];

  T.forEach(function(tp){
    tp.mem.forEach(function(m){
      if(m.c !== undefined) return;
      var lower = m.n.toLowerCase();
      for(var k = 0; k < cheatKeywords.length; k++){
        if(lower.indexOf(cheatKeywords[k]) >= 0){
          highValueVars.push(m);
          break;
        }
      }
    });
  });

  var bools = highValueVars.filter(function(m){ return m.cat === 'bool'; }).slice(0, 35);
  var nums = highValueVars.filter(function(m){ return m.cat === 'num'; }).slice(0, 35);
  var funcs = highValueVars.filter(function(m){ return m.kind === 'method'; }).slice(0, 35);

  if(bools.length){
    h += '<h2>// ' + tr('cheatBools') + ' (' + bools.length + ')</h2><div class="list">';
    bools.forEach(function(m){ h += rowHTML(m, true); });
    h += '</div>';
  }

  if(nums.length){
    h += '<h2>// ' + tr('cheatNums') + ' (' + nums.length + ')</h2><div class="list">';
    nums.forEach(function(m){ h += rowHTML(m, true); });
    h += '</div>';
  }

  if(funcs.length){
    h += '<h2>// ' + tr('cheatMethods') + ' (' + funcs.length + ')</h2><div class="list">';
    funcs.forEach(function(m){ h += rowHTML(m, true); });
    h += '</div>';
  }

  return h;
}

// TAB 4: Namespaces
function modulesHTML(){
  var h = '<h2>// ' + tr('nsTitle') + ' (' + NSLIST.length + ')</h2><div class="nsgrid">';
  NSLIST.forEach(function(n){
    h += '<div class="nsc" data-ns="' + esc(n) + '"><b>' + esc(n || '(GLOBAL)') + '</b><span>' + nsCount[n] + '</span></div>';
  });
  return h + '</div>';
}

// Class View
function typeHTML(tp){
  var h = '<button class="back" data-back="1">' + tr('back') + '</button><div class="th"><h1>' + nsPart(tp) + esc(tp.n) + (tp.g ? '&lt;' + esc(tp.g) + '&gt;' : '') + ' ' + kindBadge(tp) + '</h1>';
  var ch = chainOf(tp), rel = '';
  if(ch.length){
    rel += tr('ext') + ': ' + ch.map(function(c){ return c.t ? '<span class="lnk" data-open="' + c.t.id + '">' + esc(short(c.name)) + '</span>' : '<span>' + esc(short(c.name)) + '</span>'; }).join(' › ');
  }
  if(tp.if && tp.if.length) rel += (rel ? ' &nbsp;·&nbsp; ' : '') + tp.if.map(function(n){ var t = byFull.get(strip(n)); return t ? '<span class="lnk" data-open="' + t.id + '">' + esc(short(n)) + '</span>' : esc(short(n)); }).join(', ');
  if(rel) h += '<div class="rel">' + rel + '</div>';

  var der = T.filter(function(x){ return x.b && strip(x.b) === tp.f; });
  if(der.length) h += '<div class="rel">' + tr('derived') + ' (' + der.length + '): ' + der.slice(0, 25).map(function(x){ return '<span class="lnk" data-open="' + x.id + '">' + esc(x.n) + '</span>'; }).join(', ') + (der.length > 25 ? ' …' : '') + '</div>';
  if(tp.at && tp.at.length) h += '<div class="rel">' + tp.at.map(function(a){ return '<span class="tg at">[' + esc(a) + ']</span>'; }).join(' ') + '</div>';
  h += '</div>';

  if(tp.k !== 'enum'){
    var hs = holders(tp), box = '';
    hs.slice(0, 8).forEach(function(x){
      var m = x.m, full = tfull(m.o) + '.' + m.n;
      box += '<div class="hold"><code>' + esc(full) + '</code><span class="tg">[' + esc(short(m.ty)) + ']</span>' + (x.coll ? '<span class="tg">[' + tr('collection') + ']</span>' : '') +
        '<span class="tg ' + (isPub(m.a) ? 'pub' : '') + '">[' + ACC[m.a].toUpperCase() + ']</span><div class="act" style="opacity:1"><button data-copy="' + esc(full) + '">' + tr('cName') + '</button></div></div>';
    });
    if(isUnity(tp)){
      var fo = 'UnityEngine.Object.FindObjectOfType<' + tfull(tp) + '>()';
      box += '<div class="hold"><code>' + esc(fo) + '</code><span class="tg">[' + tr('unity') + ']</span><div class="act" style="opacity:1"><button data-copy="' + esc(fo) + '">' + tr('cName') + '</button></div></div>';
    }
    if(!box) box = '<div class="info">' + tr('noHolder') + '</div>';
    h += '<div class="box"><h3>// ' + tr('howGet') + '</h3>'+ box + '</div>';
  }

  var mems = tp.mem.slice();
  if(S.inh && tp.k !== 'enum') ch.forEach(function(c){ if(c.t) mems = mems.concat(c.t.mem); });

  var tabs = tp.k === 'enum' ? [['consts', tr('enumVals')]] : [['values', tr('values')], ['fields', tr('fields')], ['props', tr('props')], ['methods', tr('methods')], ['events', tr('events')]];
  function pick(tab){
    return mems.filter(function(m){
      if(tab === 'consts') return m.c !== undefined;
      if(m.c !== undefined) return false;
      if(tab === 'values') return m.kind === 'field' || m.kind === 'prop';
      if(tab === 'fields') return m.kind === 'field';
      if(tab === 'props') return m.kind === 'prop';
      if(tab === 'methods') return m.kind === 'method';
      return m.kind === 'event';
    });
  }

  if(!tabs.some(function(t){ return t[0] === S.tab; })) S.tab = tabs[0][0];
  h += '<div class="tabs">' + tabs.map(function(t){ return '<div class="tab' + (S.tab === t[0] ? ' on' : '') + '" data-tab="' + t[0] + '">[' + t[1] + ']<small>' + pick(t[0]).length + '</small></div>'; }).join('') + '</div>';
  h += '<div class="tbar"><input type="search" id="tq" placeholder="' + tr('filter') + '" value="' + esc(S.tq) + '">' +
    (tp.k === 'enum' ? '' : '<label class="hud-switch"><input type="checkbox" id="inh"' + (S.inh ? ' checked' : '') + '><span class="sw-mark"></span><span>' + tr('inherit') + '</span></label>') + '</div>';

  var tk = norm(S.tq), list = pick(S.tab).filter(function(m){ return !tk || m.k.indexOf(tk) >= 0; });
  if(S.tab === 'values') list.sort(function(a, b){ return ORDER[a.cat] - ORDER[b.cat]; });

  if(!list.length) h += '<div class="empty">' + tr('nothing') + '</div>';
  else {
    h += '<div class="list">';
    list.slice(0, 400).forEach(function(m){ h += rowHTML(m, m.o !== tp); });
    h += '</div>';
    if(list.length > 400) h += '<div class="info">… ' + (list.length - 400) + ' (use filter)</div>';
  }
  return h;
}

/* ---------------- MAIN RENDERER ---------------- */
function render(keepFocus){
  var m = /^#t\/(\d+)/.exec(location.hash);
  var main = $('#main');
  var clearBtn = $('#clearBtn');
  if(clearBtn) clearBtn.style.display = S.q ? 'block' : 'none';

  $$('.nav-tab').forEach(function(tab){
    tab.classList.toggle('on', tab.dataset.nav === S.nav && !m);
  });

  if(m && T[+m[1]]){
    S.lastTid = +m[1];
    updateChips(null);
    main.innerHTML = typeHTML(T[+m[1]]);
    var tq = $('#tq');
    if(keepFocus === 'tq' && tq){
      tq.focus();
      tq.setSelectionRange(tq.value.length, tq.value.length);
    } else window.scrollTo(0, 0);

  } else if(searching()){
    S.nav = 'explorer';
    $$('.nav-tab').forEach(function(tab){
      tab.classList.toggle('on', tab.dataset.nav === 'explorer');
    });
    main.innerHTML = resultsHTML();

  } else if(S.nav === 'graph'){
    updateChips({ bool: totals.b, num: totals.n, other: '', method: totals.f });
    main.innerHTML = graphViewHTML();
    initGraphCanvas();
    updateBreadcrumbsHUD();

  } else if(S.nav === 'cheats'){
    main.innerHTML = cheatHooksHTML();

  } else if(S.nav === 'modules'){
    main.innerHTML = modulesHTML();

  } else {
    main.innerHTML = resultsHTML();
  }
}

function toast(t){
  var e = $('#toast');
  e.textContent = t;
  e.className = 'hud-toast on';
  clearTimeout(toast._t);
  toast._t = setTimeout(function(){ e.className = 'hud-toast'; }, 1800);
}

function copy(text){
  function done(){
    toast(tr('copied') + ':\n' + (text.length > 140 ? text.slice(0, 140) + '…' : text));
  }
  if(navigator.clipboard && navigator.clipboard.writeText){
    navigator.clipboard.writeText(text).then(done, function(){ fb(); });
  } else fb();

  function fb(){
    var a = document.createElement('textarea');
    a.value = text;
    document.body.appendChild(a);
    a.select();
    try { document.execCommand('copy'); } catch(e){}
    document.body.removeChild(a);
    done();
  }
}

function goList(){
  if(/^#t\//.test(location.hash)) location.hash = '#list';
  else render();
}

/* ---------------- DOM EVENT LISTENERS ---------------- */
document.addEventListener('click', function(e){
  var navTab = e.target.closest('[data-nav]');
  if(navTab){
    S.nav = navTab.dataset.nav;
    if(location.hash.indexOf('#t/') === 0) location.hash = '';
    render();
    return;
  }

  if(e.target.id === 'clearBtn'){
    S.q = '';
    $('#q').value = '';
    goList();
    return;
  }

  var crumbBtn = e.target.closest('[data-crumb]');
  if(crumbBtn){
    var targetIdx = +crumbBtn.dataset.crumb;
    while(G_STATE.path.length - 1 > targetIdx){
      G_STATE.path.pop();
    }
    G_STATE.level = targetIdx;
    updateBreadcrumbsHUD();
    var lastEntity = G_STATE.path[G_STATE.path.length - 1];
    buildGraphData(G_STATE.level, lastEntity ? lastEntity.entity : null);
    return;
  }

  if(e.target.id === 'graphBackBtn'){
    zoomOutOneLevel();
    return;
  }

  if(e.target.id === 'resetGraphBtn'){
    buildGraphData(G_STATE.level, G_STATE.path[G_STATE.path.length - 1] ? G_STATE.path[G_STATE.path.length - 1].entity : null);
    return;
  }

  if(e.target.id === 'expandGraphBtn'){
    G_STATE.isTall = !G_STATE.isTall;
    var c = $('#graphCanvas');
    if(c){
      c.style.height = G_STATE.isTall ? '760px' : '560px';
      var rect = c.getBoundingClientRect();
      G_STATE.w = rect.width;
      G_STATE.h = rect.height;
      c.width = G_STATE.w * G_STATE.dpr;
      c.height = G_STATE.h * G_STATE.dpr;
      G_STATE.ctx.scale(G_STATE.dpr, G_STATE.dpr);
    }
    return;
  }

  var t = e.target.closest('[data-open],[data-act],[data-copy],[data-more],[data-exp],[data-chip],[data-q],[data-ns],[data-quick],[data-tab],[data-back]');
  if(!t) return;

  if(t.dataset.open !== undefined){
    location.hash = '#t/' + t.dataset.open;
    return;
  }
  if(t.dataset.act){
    var m = T[+t.dataset.t].mem[+t.dataset.i], a = t.dataset.act;
    copy(a === 'name' ? m.n : a === 'cs' ? csSnippet(m) : harmonySnippet(m));
    return;
  }
  if(t.dataset.copy !== undefined){
    copy(t.dataset.copy);
    return;
  }
  if(t.dataset.more){ S.limit += 30; render(); return; }
  if(t.dataset.exp){ S.exp[t.dataset.exp] = 1; render(); return; }
  if(t.dataset.chip){ S.chips[t.dataset.chip] = !S.chips[t.dataset.chip]; S.limit = 30; goList(); return; }
  if(t.dataset.q !== undefined){ S.q = t.dataset.q; $('#q').value = S.q; S.limit = 30; goList(); return; }
  if(t.dataset.ns !== undefined){ S.ns = t.dataset.ns; syncSel(); S.limit = 30; goList(); return; }
  if(t.dataset.quick){
    S.chips = { bool: false, num: false, other: false, method: false };
    S.chips[t.dataset.quick] = true;
    S.limit = 30;
    goList();
    return;
  }
  if(t.dataset.tab){ S.tab = t.dataset.tab; render(); return; }
  if(t.dataset.back){
    if(history.length > 1) history.back();
    else location.hash = '#list';
    return;
  }
});

document.addEventListener('input', function(e){
  if(e.target.id === 'q'){
    clearTimeout(render._t);
    render._t = setTimeout(function(){
      S.q = e.target.value;
      S.limit = 30;
      goList();
    }, 110);
  } else if(e.target.id === 'tq'){
    S.tq = e.target.value;
    render('tq');
  }
});

document.addEventListener('change', function(e){
  var id = e.target.id;
  if(id === 'fPub') S.pub = e.target.checked;
  else if(id === 'fStat') S.stat = e.target.checked;
  else if(id === 'fCst') S.cst = e.target.checked;
  else if(id === 'nsSel') S.ns = e.target.value === '' ? null : (e.target.value === '\u0000' ? '' : e.target.value);
  else if(id === 'asmSel') S.asm = +e.target.value;
  else if(id === 'inh'){ S.inh = e.target.checked; render('tq'); return; }
  else return;

  S.limit = 30;
  goList();
});

document.addEventListener('keydown', function(e){
  if(e.key === '/' && document.activeElement.tagName !== 'INPUT'){
    e.preventDefault();
    $('#q').focus();
  }
  if(e.key === 'Escape' && document.activeElement.id === 'q'){
    S.q = '';
    $('#q').value = '';
    goList();
  }
});

window.addEventListener('hashchange', function(){ render(); });

$('#langBtn').addEventListener('click', function(){
  lang = lang === 'ru' ? 'en' : 'ru';
  try { localStorage.setItem('dnl', lang); } catch(e){}
  applyLang();
  render();
});

function syncSel(){
  var v = S.ns === null ? '' : (S.ns === '' ? '\u0000' : S.ns);
  $('#nsSel').value = v;
}

function applyLang(){
  document.documentElement.lang = lang;
  $('#q').placeholder = tr('ph');
  document.querySelectorAll('[data-i]').forEach(function(el){
    el.textContent = tr(el.dataset.i);
  });
  $('#langBtn').textContent = lang === 'ru' ? 'EN' : 'RU';

  var o = '<option value="">' + tr('allNs') + '</option>';
  NSLIST.forEach(function(n){
    o += '<option value="' + esc(n === '' ? '\u0000' : n) + '">' + esc(n || '(GLOBAL)') + ' [' + nsCount[n] + ']</option>';
  });
  $('#nsSel').innerHTML = o;
  syncSel();

  var a = '<option value="-1">' + tr('allAsm') + '</option>';
  (D.asms || []).forEach(function(x, i){
    a += '<option value="' + i + '">' + esc(x.n) + '</option>';
  });
  $('#asmSel').innerHTML = a;
  $('#asmSel').value = String(S.asm);
  $('#asmSel').style.display = (D.asms && D.asms.length > 1) ? '' : 'none';
}

// Initial Header Setup
$('#gameName').textContent = D.game || 'UNKNOWN';
var tickerEl = $('#hudTicker');
if(tickerEl){
  tickerEl.innerHTML = 'TYPES: <span>' + totals.t + '</span> · FLAGS: <span>' + totals.b + '</span> · STATS: <span>' + totals.n + '</span>';
}
document.title = 'DNMETA // ' + (D.game || 'TARGET_REPORT');

applyLang();
render();

})();
