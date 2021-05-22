/* PrismJS 1.20.0
https://prismjs.com/download.html#themes=prism&languages=clike+c+lua
*/
var _self = "undefined" != typeof window ? window : "undefined" != typeof WorkerGlobalScope && self instanceof WorkerGlobalScope ? self : {}, Prism = function (u) { var c = /\blang(?:uage)?-([\w-]+)\b/i, n = 0, C = { manual: u.Prism && u.Prism.manual, disableWorkerMessageHandler: u.Prism && u.Prism.disableWorkerMessageHandler, util: { encode: function e(n) { return n instanceof _ ? new _(n.type, e(n.content), n.alias) : Array.isArray(n) ? n.map(e) : n.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/\u00a0/g, " ") }, type: function (e) { return Object.prototype.toString.call(e).slice(8, -1) }, objId: function (e) { return e.__id || Object.defineProperty(e, "__id", { value: ++n }), e.__id }, clone: function t(e, r) { var a, n, l = C.util.type(e); switch (r = r || {}, l) { case "Object": if (n = C.util.objId(e), r[n]) return r[n]; for (var i in a = {}, r[n] = a, e) e.hasOwnProperty(i) && (a[i] = t(e[i], r)); return a; case "Array": return n = C.util.objId(e), r[n] ? r[n] : (a = [], r[n] = a, e.forEach(function (e, n) { a[n] = t(e, r) }), a); default: return e } }, getLanguage: function (e) { for (; e && !c.test(e.className);)e = e.parentElement; return e ? (e.className.match(c) || [, "none"])[1].toLowerCase() : "none" }, currentScript: function () { if ("undefined" == typeof document) return null; if ("currentScript" in document) return document.currentScript; try { throw new Error } catch (e) { var n = (/at [^(\r\n]*\((.*):.+:.+\)$/i.exec(e.stack) || [])[1]; if (n) { var t = document.getElementsByTagName("script"); for (var r in t) if (t[r].src == n) return t[r] } return null } } }, languages: { extend: function (e, n) { var t = C.util.clone(C.languages[e]); for (var r in n) t[r] = n[r]; return t }, insertBefore: function (t, e, n, r) { var a = (r = r || C.languages)[t], l = {}; for (var i in a) if (a.hasOwnProperty(i)) { if (i == e) for (var o in n) n.hasOwnProperty(o) && (l[o] = n[o]); n.hasOwnProperty(i) || (l[i] = a[i]) } var s = r[t]; return r[t] = l, C.languages.DFS(C.languages, function (e, n) { n === s && e != t && (this[e] = l) }), l }, DFS: function e(n, t, r, a) { a = a || {}; var l = C.util.objId; for (var i in n) if (n.hasOwnProperty(i)) { t.call(n, i, n[i], r || i); var o = n[i], s = C.util.type(o); "Object" !== s || a[l(o)] ? "Array" !== s || a[l(o)] || (a[l(o)] = !0, e(o, t, i, a)) : (a[l(o)] = !0, e(o, t, null, a)) } } }, plugins: {}, highlightAll: function (e, n) { C.highlightAllUnder(document, e, n) }, highlightAllUnder: function (e, n, t) { var r = { callback: t, container: e, selector: 'code[class*="language-"], [class*="language-"] code, code[class*="lang-"], [class*="lang-"] code' }; C.hooks.run("before-highlightall", r), r.elements = Array.prototype.slice.apply(r.container.querySelectorAll(r.selector)), C.hooks.run("before-all-elements-highlight", r); for (var a, l = 0; a = r.elements[l++];)C.highlightElement(a, !0 === n, r.callback) }, highlightElement: function (e, n, t) { var r = C.util.getLanguage(e), a = C.languages[r]; e.className = e.className.replace(c, "").replace(/\s+/g, " ") + " language-" + r; var l = e.parentNode; l && "pre" === l.nodeName.toLowerCase() && (l.className = l.className.replace(c, "").replace(/\s+/g, " ") + " language-" + r); var i = { element: e, language: r, grammar: a, code: e.textContent }; function o(e) { i.highlightedCode = e, C.hooks.run("before-insert", i), i.element.innerHTML = i.highlightedCode, C.hooks.run("after-highlight", i), C.hooks.run("complete", i), t && t.call(i.element) } if (C.hooks.run("before-sanity-check", i), !i.code) return C.hooks.run("complete", i), void (t && t.call(i.element)); if (C.hooks.run("before-highlight", i), i.grammar) if (n && u.Worker) { var s = new Worker(C.filename); s.onmessage = function (e) { o(e.data) }, s.postMessage(JSON.stringify({ language: i.language, code: i.code, immediateClose: !0 })) } else o(C.highlight(i.code, i.grammar, i.language)); else o(C.util.encode(i.code)) }, highlight: function (e, n, t) { var r = { code: e, grammar: n, language: t }; return C.hooks.run("before-tokenize", r), r.tokens = C.tokenize(r.code, r.grammar), C.hooks.run("after-tokenize", r), _.stringify(C.util.encode(r.tokens), r.language) }, tokenize: function (e, n) { var t = n.rest; if (t) { for (var r in t) n[r] = t[r]; delete n.rest } var a = new l; return M(a, a.head, e), function e(n, t, r, a, l, i, o) { for (var s in r) if (r.hasOwnProperty(s) && r[s]) { var u = r[s]; u = Array.isArray(u) ? u : [u]; for (var c = 0; c < u.length; ++c) { if (o && o == s + "," + c) return; var g = u[c], f = g.inside, h = !!g.lookbehind, d = !!g.greedy, v = 0, p = g.alias; if (d && !g.pattern.global) { var m = g.pattern.toString().match(/[imsuy]*$/)[0]; g.pattern = RegExp(g.pattern.source, m + "g") } g = g.pattern || g; for (var y = a.next, k = l; y !== t.tail; k += y.value.length, y = y.next) { var b = y.value; if (t.length > n.length) return; if (!(b instanceof _)) { var x = 1; if (d && y != t.tail.prev) { g.lastIndex = k; var w = g.exec(n); if (!w) break; var A = w.index + (h && w[1] ? w[1].length : 0), P = w.index + w[0].length, S = k; for (S += y.value.length; S <= A;)y = y.next, S += y.value.length; if (S -= y.value.length, k = S, y.value instanceof _) continue; for (var O = y; O !== t.tail && (S < P || "string" == typeof O.value && !O.prev.value.greedy); O = O.next)x++, S += O.value.length; x--, b = n.slice(k, S), w.index -= k } else { g.lastIndex = 0; var w = g.exec(b) } if (w) { h && (v = w[1] ? w[1].length : 0); var A = w.index + v, w = w[0].slice(v), P = A + w.length, E = b.slice(0, A), N = b.slice(P), j = y.prev; E && (j = M(t, j, E), k += E.length), W(t, j, x); var L = new _(s, f ? C.tokenize(w, f) : w, p, w, d); if (y = M(t, j, L), N && M(t, y, N), 1 < x && e(n, t, r, y.prev, k, !0, s + "," + c), i) break } else if (i) break } } } } }(e, a, n, a.head, 0), function (e) { var n = [], t = e.head.next; for (; t !== e.tail;)n.push(t.value), t = t.next; return n }(a) }, hooks: { all: {}, add: function (e, n) { var t = C.hooks.all; t[e] = t[e] || [], t[e].push(n) }, run: function (e, n) { var t = C.hooks.all[e]; if (t && t.length) for (var r, a = 0; r = t[a++];)r(n) } }, Token: _ }; function _(e, n, t, r, a) { this.type = e, this.content = n, this.alias = t, this.length = 0 | (r || "").length, this.greedy = !!a } function l() { var e = { value: null, prev: null, next: null }, n = { value: null, prev: e, next: null }; e.next = n, this.head = e, this.tail = n, this.length = 0 } function M(e, n, t) { var r = n.next, a = { value: t, prev: n, next: r }; return n.next = a, r.prev = a, e.length++, a } function W(e, n, t) { for (var r = n.next, a = 0; a < t && r !== e.tail; a++)r = r.next; (n.next = r).prev = n, e.length -= a } if (u.Prism = C, _.stringify = function n(e, t) { if ("string" == typeof e) return e; if (Array.isArray(e)) { var r = ""; return e.forEach(function (e) { r += n(e, t) }), r } var a = { type: e.type, content: n(e.content, t), tag: "span", classes: ["token", e.type], attributes: {}, language: t }, l = e.alias; l && (Array.isArray(l) ? Array.prototype.push.apply(a.classes, l) : a.classes.push(l)), C.hooks.run("wrap", a); var i = ""; for (var o in a.attributes) i += " " + o + '="' + (a.attributes[o] || "").replace(/"/g, "&quot;") + '"'; return "<" + a.tag + ' class="' + a.classes.join(" ") + '"' + i + ">" + a.content + "</" + a.tag + ">" }, !u.document) return u.addEventListener && (C.disableWorkerMessageHandler || u.addEventListener("message", function (e) { var n = JSON.parse(e.data), t = n.language, r = n.code, a = n.immediateClose; u.postMessage(C.highlight(r, C.languages[t], t)), a && u.close() }, !1)), C; var e = C.util.currentScript(); function t() { C.manual || C.highlightAll() } if (e && (C.filename = e.src, e.hasAttribute("data-manual") && (C.manual = !0)), !C.manual) { var r = document.readyState; "loading" === r || "interactive" === r && e && e.defer ? document.addEventListener("DOMContentLoaded", t) : window.requestAnimationFrame ? window.requestAnimationFrame(t) : window.setTimeout(t, 16) } return C }(_self); "undefined" != typeof module && module.exports && (module.exports = Prism), "undefined" != typeof global && (global.Prism = Prism);

// Prism.languages.wren
(function (Prism) {

    // Multiline based on prism-rust.js
    var multilineComment = /\/\*(?:[^*/]|\*(?!\/)|\/(?!\*)|<self>)*\*\//.source;
    for (var i = 0; i < 2; i++) {
        // Supports up to 4 levels of nested comments
        multilineComment = multilineComment.replace(/<self>/g, function () { return multilineComment; });
    }
    multilineComment = multilineComment.replace(/<self>/g, function () { return /[^\s\S]/.source; });

    var wren = {
        // Multiline comments in Wren can have nested multiline comments
        // Comments: // and /* */
        'comment': [
            {
                pattern: RegExp(/(^|[^\\])/.source + multilineComment),
                lookbehind: true,
                greedy: true
            },
            {
                pattern: /(^|[^\\:])\/\/.*/,
                lookbehind: true,
                greedy: true
            }
        ],

        // Triple quoted strings are multiline but cannot have interpolation (raw strings)
        // Based on prism-python.js
        'triple-quoted-string': {
            pattern: /(""")[\s\S]*?\1/iu,
            greedy: true,
            alias: 'string'
        },

        // A single quote string is multiline and can have interpolation (similar to JS backticks ``)
        'string': {
            pattern: /"(?:\\[\s\S]|%\((?:[^()]|\((?:[^()]|\([^)]*\))*\))+\)|(?!%\()[^\\"])*"/u,
            greedy: true,
            inside: {}
            // Interpolation defined at the end of this function
        },

        'boolean': /\b(?:true|false)\b/,
        'number': /\b0x[\da-f]+\b|(?:\b\d+(?:\.\d*)?|\B\.\d+)(?:e[+-]?\d+)?/i,
        'null': {
            pattern: /\bnull\b/,
            alias: 'keyword'
        },

        // Highlight predefined classes and wren_cli classes as builtin
        'builtin': /\b(?:Num|System|Object|Sequence|List|Map|Bool|String|Range|Fn|Fiber|Meta|Random|File|Directory|Stat|Stdin|Stdout|Platform|Process|Scheduler|Timer)\b/,

        // Attributes are special keywords to add meta data to classes
        'attribute': [
            // #! attributes are stored in class properties
            // #!myvar = true
            // #attributes are not stored and dismissed at compilation
            {
                pattern: /#!?[ \t\u3000]*[A-Za-z_\d]+\b/u,
                alias: 'keyword'
            },
        ],
        // #!/usr/bin/env wren on the first line
        'hashbang': [
            {
            pattern: /#!\/[\S \t\u3000]+/u,
            greedy:true,
            alias:'constant'
            }
        ],
        'class-name': [
            {
                // class definition
                // class Meta {}
                pattern: /(\b(?:class)\s+)[\w.\\]+/i,
                lookbehind: true,
                inside: {
                    'punctuation': /[.\\]/
                }
            },
            {
                // A class must always start with an uppercase.
                // File.read
                pattern: /\b[A-Z][a-z\d_]*\b/,
                lookbehind:true,
                inside: {
                'punctuation': /[.\\]/
                }
            }
        ],

        // A constant can be a variable, class, property or method. Just named in all uppercase letters
        'constant': /\b[A-Z][A-Z\d_]*\b/,

        'keyword': /\b(?:if|else|while|for|return|in|is|as|null|break|continue|foreign|construct|static|var|class|this|super|#!|#|import)\b/,

        // Functions can be Class.method()
        'function': /(?!\d)\w+(?=\s*(?:[({]))/,

        // Traditional operators but adding .. and ... for Ranges e.g.: 1..2
        // Based on prism-lua.js
        'operator': [
            /[-+*%^&|]|\/\/?|<[<=]?|>[>=]?|[=~]=?/,
            {
                // Match ".." but don't break "..."
                pattern: /(^|[^.])\.\.(?!\.)/,
                lookbehind: true
            }
        ],
        // Traditional punctuation although ; is not used in Wren
        'punctuation': /[\[\](){},;]|\.+|:+/,
        'variable': /[a-zA-Z_\d]\w*\b/,
    };

    // Based on prism-javascript.js interpolation
    // "%(interpolation)"
    var stringInside = {
        'template-punctuation': {
        pattern: /^"|"$/,
        alias: 'string'
        },
        'interpolation': {
        pattern: /((?:^|[^\\])(?:\\{2})*)%\((?:[^()]|\((?:[^()]|\([^)]*\))*\))+\)/,
        lookbehind: true,
        inside: {
            'interpolation-punctuation': {
            pattern: /^%(|)$/,
            alias: 'punctuation'
            },
            rest: wren
        }
        },
        'string': /[\s\S]+/iu
    };

    // Only single quote strings can have interpolation
    wren['string'].inside = stringInside;

    Prism.languages.wren = wren;
})(Prism);

// backwards compatibility
Prism.languages.lua = Prism.languages.wren;

// default Prism languages
Prism.languages.clike={comment:[{pattern:/(^|[^\\])\/\*[\s\S]*?(?:\*\/|$)/,lookbehind:!0},{pattern:/(^|[^\\:])\/\/.*/,lookbehind:!0,greedy:!0}],string:{pattern:/(["'])(?:\\(?:\r\n|[\s\S])|(?!\1)[^\\\r\n])*\1/,greedy:!0},"class-name":{pattern:/(\b(?:class|interface|extends|implements|trait|instanceof|new)\s+|\bcatch\s+\()[\w.\\]+/i,lookbehind:!0,inside:{punctuation:/[.\\]/}},keyword:/\b(?:if|else|while|do|for|return|in|instanceof|function|new|try|throw|catch|finally|null|break|continue)\b/,boolean:/\b(?:true|false)\b/,function:/\w+(?=\()/,number:/\b0x[\da-f]+\b|(?:\b\d+\.?\d*|\B\.\d+)(?:e[+-]?\d+)?/i,operator:/[<>]=?|[!=]=?=?|--?|\+\+?|&&?|\|\|?|[?*/~^%]/,punctuation:/[{}[\];(),.:]/};
Prism.languages.c=Prism.languages.extend("clike",{comment:{pattern:/\/\/(?:[^\r\n\\]|\\(?:\r\n?|\n|(?![\r\n])))*|\/\*[\s\S]*?(?:\*\/|$)/,greedy:!0},"class-name":{pattern:/(\b(?:enum|struct)\s+(?:__attribute__\s*\(\([\s\S]*?\)\)\s*)?)\w+/,lookbehind:!0},keyword:/\b(?:__attribute__|_Alignas|_Alignof|_Atomic|_Bool|_Complex|_Generic|_Imaginary|_Noreturn|_Static_assert|_Thread_local|asm|typeof|inline|auto|break|case|char|const|continue|default|do|double|else|enum|extern|float|for|goto|if|int|long|register|return|short|signed|sizeof|static|struct|switch|typedef|union|unsigned|void|volatile|while)\b/,function:/[a-z_]\w*(?=\s*\()/i,operator:/>>=?|<<=?|->|([-+&|:])\1|[?:~]|[-+*/%&|^!=<>]=?/,number:/(?:\b0x(?:[\da-f]+\.?[\da-f]*|\.[\da-f]+)(?:p[+-]?\d+)?|(?:\b\d+\.?\d*|\B\.\d+)(?:e[+-]?\d+)?)[ful]*/i}),Prism.languages.insertBefore("c","string",{macro:{pattern:/(^\s*)#\s*[a-z]+(?:[^\r\n\\/]|\/(?!\*)|\/\*(?:[^*]|\*(?!\/))*\*\/|\\(?:\r\n|[\s\S]))*/im,lookbehind:!0,greedy:!0,alias:"property",inside:{string:[{pattern:/^(#\s*include\s*)<[^>]+>/,lookbehind:!0},Prism.languages.c.string],comment:Prism.languages.c.comment,directive:{pattern:/^(#\s*)[a-z]+/,lookbehind:!0,alias:"keyword"}}},constant:/\b(?:__FILE__|__LINE__|__DATE__|__TIME__|__TIMESTAMP__|__func__|EOF|NULL|SEEK_CUR|SEEK_END|SEEK_SET|stdin|stdout|stderr)\b/}),delete Prism.languages.c.boolean;