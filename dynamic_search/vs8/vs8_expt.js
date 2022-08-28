/** make trial permutations
 * @returns {number} */
function calc_nperm(ivs) {
    let nperm = 1;
    for (key in ivs) nperm *= ivs[key].length;
    return nperm;
}

/** make trial permutations
 * @returns {Array} */
function permu_trials(ivs, nt) {
    let nperm = calc_nperm(ivs);
    if (nt % nperm) throw "Number of trials is not permutation of IVs";
    let trials = new Array(nt);
    for (let ti = 0; ti < trials.length; ti++) {
        let carry = ti;
        trials[ti] = {};
        let ivs_entries = Object.entries(ivs);
        ivs_entries.reverse();
        let trialvals = {};
        for (let [key, value] of ivs_entries) {
            let nl = value.length;
            let remainder = carry % nl;
            trialvals[key] = value[remainder];
            carry -= remainder;
            carry /= nl;
        }
        for (key in ivs) trials[ti][key] = trialvals[key];
    }
    return trials;
}

/** produce the permutation of conditions 
 * @returns {Array}
 */
function permu_conds(ivs) {
    let nperm = calc_nperm(ivs);
    return permu_trials(ivs, nperm);
}

/**
 * permutation of array
 * @param {Array<any>} arr 
 */
function permu_arr(arr) {
    let permutation = deepcopy(arr);
    let length = permutation.length,
        result = [permutation.slice()],
        c = new Array(length).fill(0),
        i = 1,
        k, p;

    while (i < length) {
        if (c[i] < i) {
            k = i % 2 && c[i];
            p = permutation[i];
            permutation[i] = permutation[k];
            permutation[k] = p;
            ++c[i];
            i = 1;
            result.push(permutation.slice());
        } else {
            c[i] = 0;
            ++i;
        }
    }
    return result;
}


/**
 * 
 * @param {Object} cond 
 * @param {Object} trial 
 * @returns {Boolean}
 */
function match_cond(cond, trial) {
    let match = true;
    for (let key in cond) match = match && cond[key] == trial[key];
    return match;
}

/** gen trials, support practice trials and shuffled
 * @returns {Array} */
function gen_trials(ivs, nt, npt = 0) {
    let nperm = calc_nperm(ivs);

    let ptrials = [];
    if (npt > 0) {
        let nptcarry = npt;
        if (nptcarry % nperm);
        nptcarry += nperm - npt % nperm;
        ptrials = permu_trials(ivs, nptcarry);
        shuffle(ptrials);
        ptrials = ptrials.slice(0, npt);
        ptrials.forEach(function (trial, ti) { trial.ti = ti - npt; });
    }
    let trials = permu_trials(ivs, nt);
    shuffle(trials);
    trials.forEach(function (trial, ti) { trial.ti = ti; });
    return ptrials.concat(trials);
}
/** search_ring
 *  
 * holder size, ring radius, cx, cy, options={phase:-pi/2,shuffle:true,ssize:hsize} 

 * return an array of coordinates
 * @returns {Array<Array<number>> }
 */
function search_ring(hsize, cx, cy, radius, options = { phase: -Math.PI / 2, shuffle: true }) {
    let out = [];
    for (let i = 0; i < hsize; i++) {
        let pos_angle = i / hsize * Math.PI * 2 + options.phase;
        out.push([cx + Math.cos(pos_angle) * radius, cy + Math.sin(pos_angle) * radius]);
    }
    if (options.shuffle) shuffle(out);
    if ('ssize' in options) out.splice(options.ssize);
    return out;
}

/** search_table
 *  
 * mw and mh is for calculating the margin, which is calculated from item center. it should be slightly larger than half the item width and height.
 *ncol, nrow, cx, cy, w, h, mw, mh, options={shuffle:true,ssize:(ncol*nrow)}
 * 
 * return an array of coordinates
 * @returns {Array<Array<number>> }
 */
function search_table(ncol, nrow, cx, cy, w, h, mw, mh, options = { shuffle: true }) {
    //check param validity
    if (mw * 2 > w / ncol) throw "mw too wide";
    if (mh * 2 > h / nrow) throw "mh too tall";
    //set table
    let out = [];
    for (let i = 0; i < ncol; i++)
        for (let j = 0; j < nrow; j++) {
            let pos = [
                (cx - w / 2) + i / ncol * w + mw + Math.random() * (w / ncol - mw * 2),
                (cy - h / 2) + j / nrow * h + mh + Math.random() * (h / nrow - mh * 2)
            ];
            out.push(pos);
        }

    if (options.shuffle) shuffle(out);
    if ('ssize' in options) out.splice(options.ssize);
    return out;
}

/**
 * make multi-ring search configuration with provided ssize and radius for each ring 
 * @param {Array<number>} ring_sss 
 * @param {number} cx 
 * @param {number} cy 
 * @param {Array<number>} ring_rs 
 * @param {number} margin_r 
 * @param {object} options 
 */
 function search_multiring(ring_sss, cx, cy, ring_rs, margin_r, options = {}) {
    if (ring_sss.length != ring_rs.length) throw "ring number not match";
    options = Object.assign({ shuffle: true, random_offset: true }, options);
    let nr = ring_sss.length;
    let itemposs = [];
    for (let ri = 0; ri < nr; ri++) {
        let ringssize = ring_sss[ri];
        let ringr = ring_rs[ri];
        margin_rrad = Math.asin(margin_r / ringr);
        let partitionrad = 2 * Math.PI / ringssize - 2 * margin_rrad;
        let offset = 0;
        if (options.random_offset) offset = 2 * Math.PI * Math.random();
        let ring_itempos = [];
        for (let itemi = 0; itemi < ringssize; itemi++) {
            let itemrad = offset + itemi * (2 * margin_rrad + partitionrad) + margin_rrad + Math.random() * partitionrad;
            ring_itempos.push([cx + Math.sin(itemrad) * ringr, cy - Math.cos(itemrad) * ringr]);
        }
        itemposs.push(ring_itempos);
    }
    let itempos = [];
    if ('target_ring' in options) {
        let tr = options.target_ring;
        if (options.shuffle) shuffle(itemposs[tr]);
        let tarpos = itemposs[tr].splice(0, 1);
        for (let ri = 0; ri < nr; ri++) itempos = itempos.concat(itemposs[ri]);
        if (options.shuffle) shuffle(itempos);
        itempos = tarpos.concat(itempos);
    } else {
        for (let ri = 0; ri < nr; ri++) itempos = itempos.concat(itemposs[ri]);
        if (options.shuffle) shuffle(itempos);
    }
    return itempos;
}
/**
 * calculate multi-ring search configuration with provided total ssize and automatically determine and return the ssize and radius for each ring
 * @param {number} ssize 
 * @param {number} nrings 
 * @param {number} inner_r 
 * @param {number} outer_r 
 * @param {number} margin_r 
 */
function search_multiring_autoconf(ssize, nrings, inner_r, outer_r, margin_r) {
    //first, determine the radius of each rings from inner_r and outer_r
    let sep = (outer_r - inner_r) / (nrings - 1);
    if (sep < 2 * margin_r) throw "rings too packed";
    //determine total usable place
    let max_sss = [];
    let max_ssfs = [];
    let ring_rs = [];
    for (let ri = 0; ri < nrings; ri++) {
        let cur_r = inner_r + ri * sep;
        ring_rs.push(cur_r);
        margin_rad = Math.asin(margin_r / cur_r);
        max_sss.push(Math.floor(Math.PI / margin_rad));
        max_ssfs.push(Math.PI / margin_rad);
    }
    let max_ss = sum(max_sss);
    let max_ssf = sum(max_ssfs);
    if (ssize > max_ss) throw "ssize too large";
    //distribute ssize to rings
    let assigned_sss = [];
    for (let ri = 0; ri < nrings; ri++) assigned_sss.push(Math.floor(ssize * max_ssfs[ri] / max_ssf));
    while (sum(assigned_sss) < ssize) {
        let maxdiff = 0;
        let maxdiffri;
        for (let ri = 0; ri < nrings; ri++) if (assigned_sss[ri] < max_sss[ri]) {
            let curdiff = ssize * max_ssfs[ri] / max_ssf - assigned_sss[ri];
            if (curdiff > maxdiff) {
                maxdiff = curdiff;
                maxdiffri = ri;
            }
        }
        assigned_sss[maxdiffri]++;
    }
    return [assigned_sss, ring_rs];
}
/**
 * calculate multi-ring search configuration with provided total ssize and automatically determine the ssize and radius for each ring
 * @param {number} ssize 
 * @param {number} cx 
 * @param {number} cy 
 * @param {number} nrings 
 * @param {number} inner_r 
 * @param {number} outer_r 
 * @param {number} margin_r 
 * @param {object} options 
 */
function search_multiring_auto(ssize, cx, cy, nrings, inner_r, outer_r, margin_r, options = {}) {
    let [sss, ring_rs] = search_multiring_autoconf(ssize, nrings, inner_r, outer_r, margin_r);
    return search_multiring(sss, cx, cy, ring_rs, margin_r, options);
}

/** random sample an element from an array */
function sample(arr) {
    return arr[Math.floor(Math.random() * arr.length)];
}

//shorthands
/**
 * shorthand for calculating mean rt. assume rt is stored in trial.rt 
 * @param {Array<Object>} trials 
 */
function meanrt(trials) {
    return trials.reduce((acc, trial) => acc + trial.rt, 0) / trials.length;
}
/**
 * shorthand for calculating accuracy, assume accuracy is stored in trial.correct
 * @param {*} trials 
 */
function accuracy(trials) {
    return trials.filter(trial => trial.correct).length / trials.length;
}
/**
 * shorthand for calculating correct rt and accuracy, assume accuracy is stored in trial.correct
 * @param {*} trials 
 * @returns {{corr_rt:number,accuracy:number}}
 */
function calc_rtacc(trials) {
    let correct_trials = trials.filter(trial => trial.correct);
    return {
        corr_rt: meanrt(correct_trials),
        accuracy: correct_trials.length / trials.length
    };
}
/**
 * basic rt filter to check for negative ti, rt<200, and rt>q3+2*iqr. bi is not checked
 * @param {Array<Object>} trials 
 */
function basic_rtfilter(trials) {
    let nonwarm_trials = trials.filter(trial => trial.ti >= 0);
    let rts = prop2arr(nonwarm_trials, 'rt');
    sort_numeric(rts);
    let q3 = get_percentile(rts, .75);
    let q1 = get_percentile(rts, .25);
    let ul = q3 + 2 * (q3 - q1);
    return nonwarm_trials.filter(trial => trial.rt > 200 && trial.rt < ul);
}

