const fs = require('fs');
const path = require('path');
const vs = require('./vs8_compiled.js');

const data_path = 'data';
const output_path = 'outputs';

//ana params
const output_avg = false;

//conditions
let exptcode = 'dynamic_search';
let btws = ['id','prev_order'];
let dvs = ['hitrt', 'missrate', 'fa', 'n'];

//within-subj conds
//for labels
let label_ivs = {
    prev: ['rare', 'frequent'], //0:rare, 1:frequent
};

//for matching
let ivs = {
    prev: [0, 1], //0:rare, 1:frequent
};

//match conditions
let labels = vs.permu_conds(label_ivs).map(label_cond => Object.values(label_cond).join('_'));
let conds = vs.permu_conds(ivs);

//results
let results = [];

let idset = new Set();

let datadir = fs.readdirSync(data_path);
for (let jf of datadir) if (jf.match(/\.json$/)) {
    let expt = JSON.parse(fs.readFileSync(path.join(data_path, jf)));

    if (idset.has(expt.btw.id)) {
        console.log('repeated: ' + expt.btw.id);
        continue;
    }
    idset.add(expt.btw.id);

    //ana this file
    let result = {
        btw: expt.btw,
        avg: {},
        dvs: {},
    };
    result.btw = expt.btw;
    for (let dv of dvs) result.dvs[dv] = {};

    //do basic filtering
    // let trials = vs.basic_rtfilter(expt.trials);
    let trials = expt.responses;

    //correct problem rt
    if (expt.timing_diag.keydown_ts > 86400000) {
        console.log(expt.btw.id);
        for (let trial of trials) if (trial.type == 'hit') trial.rt = (trial.ts - expt.timing_diag.keydown_ts) / 1000000000 + expt.timing_diag.perfnow_ts - trial.onset;
    }

    for (let i = 0; i < labels.length; i++) {
        let label = labels[i];
        let cond = conds[i];
        cond_trials = trials.filter(trial => vs.match_cond(cond, trial));
        //analysis stage 1
        let hit, hitrt;
        {
            let hitarr = cond_trials.filter(response => response.type == 'hit');
            hitrt = vs.prop_mean(hitarr, 'rt');
            hit = hitarr.length;
        }
        let miss = cond_trials.filter(response => response.type == 'miss').length;
        let fa = cond_trials.filter(response => response.type == 'fa').length;
        //analysis stage 2
        result.dvs.hitrt[label] = hitrt;
        result.dvs.missrate[label] = miss / (hit + miss);
        result.dvs.fa[label] = fa;
        result.dvs.n[label] = hit + miss;
    }
    //calculate averages
    if (output_avg) for (let dv of dvs) {
        result.avg[dv] = 0;
        for (let label of labels) result.avg[dv] += result.dvs[dv][label] / labels.length;
    }

    //save
    results.push(result);
}

//output to file
if (!fs.existsSync(output_path)) fs.mkdirSync(output_path);

//for each dv
for (let dv of dvs) {
    let output = '';
    {
        let headerarr = [];
        headerarr.push(...btws);
        if (output_avg) headerarr.push('avg');
        headerarr.push(...labels);
        output += headerarr.join(',') + '\n';
    }
    for (let result of results) {
        let rowarr = [];
        //between subjs
        for (let btw of btws) {
            let btwval;
            if (btw in result.btw) btwval = result.btw[btw];
            rowarr.push(btwval);
        }
        //averages for within subjs
        if (output_avg) rowarr.push(result.avg[dv]);
        //within subjs
        for (let label of labels) {
            rowarr.push(result.dvs[dv][label]);
        }
        output += rowarr.join(',') + '\n';
    }
    let outfn = path.join(output_path, [exptcode, dv].join('_') + '.csv');
    fs.writeFileSync(outfn, output);

}

