Vue.component('header-file', {
    props: ['item'],
    methods: {
        sex(str) {
            if (!str) return '';
            return str.split('\n').map(x => '<p>' + x + '</p>').join('');
        },
        replaceKeyword(str) {
            if (!str) return '';
            str = str.replace(/"(.*?)"/gm, function(r1, r2) {
                return `<span class="item-string">&quot;${r2}&quot;</span>`;
            });
            str = str.replace(/!([a-zA-Z_0-9]+)!/gm, function(r1, r2) {
                return `<span class="item-macros">${r2}</span>`;
            });
            str = str.replace(/\$([a-zA-Z_0-9]+)\$/gm, function(r1, r2) {
                return `<span class="item-var">${r2}</span>`;
            });
            str = str.replace(/%([a-zA-Z_0-9 *]+)%/gm, function(r1, r2) {
                return `<span class="item-type">${r2}</span>`;
            });
            str = str.replace(/(-?[0-9][0-9,\.]*)\b/gm, function(r1, r2) {
                return `<span class="item-number">${r2}</span>`;
            });
            str = str.replace(/~([a-zA-Z_0-9]+)~/gm, function(r1, r2) {
                return `<span class="item-function">${r2}</span>`;
            });
            str = str.replace(/#([a-zA-Z_0-9\.]+)#/gm, function(r1, r2) {
                return `<span class="item-type">${r2}</span>`;
            });

            return str;
        },
        table(params) {
            let out = `<table>`;
            for (let i = 0; i < params.length; i++) {
                out += `<tr>`;
                out += `<td style="color: #0070a3;white-space: nowrap;">${params[i][0]}</td>`;
                out += `<td style="color: #e34a7f;"><i>${params[i][1]}</i></td>`;
                out += `<td>${this.replaceKeyword(params[i][2])}</td>`;
                out += `</tr>`;
            }
            out += `</table>`;
            return out;
        },
        define(fn) {
            let out = ``;
            for (let i = 0; i < fn.params.length; i++) {
                out += `<span style="color: #0070a3;">${fn.params[i][0]}</span> <span style="color: #e34a7f;">${fn.params[i][1]}</span>`;
                if (i < fn.params.length - 1) out += ', ';
            }
            return `<span class="item-area" style="padding: 5px 10px;">
                <span style="color: #0070a3;">${fn.returnType}</span> ${fn.function}(${out});
            </span>`;
        }
    },
    template: `<div>
    <h1>{{ item.documentName }}</h1>
    <div v-if="item.fileDescription" class="function-container description" v-html="replaceKeyword(sex(item.fileDescription))"></div>
    <div class="function-container" v-for="fn in item.functionList">
        <h2>{{fn.function}}</h2>
        <h3>Define</h3>
        <div v-html="define(fn)"></div>
        <h3 v-if="fn.returnDescription">Return</h3>
        <div v-if="fn.returnDescription" v-html="replaceKeyword(fn.returnDescription)"></div>
        <h3>Params</h3>
        <div v-html="table(fn.params)"></div>
        <h3>Description</h3>
        <div class="description" v-html="replaceKeyword(sex(fn.description))"></div>
    </div>
</div>`
});

var app = new Vue({
    el: '#app',
    methods: {
        totalFunctionAmount() {
            return info.map(x => x.functionList ?x.functionList.length :0).reduce((x, p) => x + p);
        },
        search(event) {
            let val = event.target.value;
            this.filterList = JSON.parse(JSON.stringify(info));

            for (let i = 0; i < this.filterList.length; i++) {
                if (this.filterList[i].functionList) {
                    this.filterList[i].functionList = this.filterList[i].functionList.filter(x => x.function.match(val));
                } else this.filterList[i].functionList = [];
            }

            this.filterList = this.filterList.filter(x => x.functionList.length);
        }
    },
    data: {
        // headerList: [].concat(info),
        filterList: JSON.parse(JSON.stringify(info))
    }
});