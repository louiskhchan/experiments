//global expt params and vars
var d1;
var frame_interval;

async function do_upload(expt) {
    let upload_result;
    let d1upper = add({ ele: d1, tag: 'div', class: 'instructiondiv' });
    //display result summary
    {
        addhtml(d1upper, "<h2>Your experiment results:</h2>");
        let formatnum = function (n) {
            if (isNaN(n)) return '--';
            else return n.toFixed(0);
        };

        //analysis for display only
        let responses = expt.responses;

        let prev_label = ['Frequent condition', 'Rare condition'];
        let summarytable = {};
        for (let prev = 0; prev < 2; prev++) {
            let prev_responses = expt.responses.filter(response => (response.prev == prev));

            //within-subj conds
            let hitrt, hit;
            {
                let hitarr = prev_responses.filter(response => response.type == 'hit');
                hitrt = prop_mean(hitarr, 'rt');
                hit = hitarr.length;
            }
            let miss = prev_responses.filter(response => response.type == 'miss').length;
            let fa = prev_responses.filter(response => response.type == 'fa').length;

            summarytable[prev_label[prev]] = {
                'Hit RT': formatnum(hitrt) + 'ms',
                'Miss rate': formatnum((miss / (hit + miss)) * 100) + '%',
                'False alarms': formatnum(fa) + ' times'
            };
        }

        let table = addtable(d1upper, summarytable);
    }

    //upload data
    while (true) {
        addhtml(d1upper, '<h3>Uploading data...</h3>');
        upload_result = await upload({
            url: 'receive.php',
            body: JSON.stringify(expt)
        });
        if (upload_result.success) {
            addhtml(d1upper, "<h3>Upload complete! Thank you for your participation.</h3>");
            await wait_forever();
        } else {
            addhtml(d1upper, "<h3>Upload failed. Please try again.</h3>");
            let but = add({ ele: d1upper, tag: 'button', text: 'Try again' });
            await wait_event({ ele: but, type: 'click' });
        }
    }
    d1upper.remove();
}

/**@param { {context:CanvasRenderingContext2D}} c */
async function run_play(play) {
    //set up canvas
    let canvas = add({ ele: d1, tag: 'canvas', style: `width:100vw;height:100vh;position:absolute;cursor:none;background-color:white;` });

    //set up canvas and get context
    let c = expt_setup_canvas(canvas);
    let [cp, cx, cy, cw, ch, context] = [c.cp, c.cx, c.cy, c.cw, c.ch, c.context];

    let search = {
        canvas: {},
        itemr: 1.5 * cp,
    };
    search.canvas.h = 50 * cp;
    search.canvas.w = search.canvas.h * 4 / 3;
    search.canvas.x = (cw - search.canvas.w) / 2;
    search.canvas.y = (ch - search.canvas.h) / 2;

    //initialize play
    let items = [];
    for (let i = 0; i < play.ssize; i++) {
        items.push(gen_item(search));
    }

    //set up play queue according to play spec
    let actual_duration = play.duration - gplay.speed.timeout * play.numtar;
    let targets_onset = [];
    for (let i = 0; i < play.numtar; i++) {
        targets_onset.push(
            play.duration_padding +
            i * actual_duration / play.numtar + rand_between(0, actual_duration / play.numtar) +
            i * gplay.speed.timeout
        );
    }
    let tari = 0;
    let curtar = null;

    //response handler
    let onresponse = function (e) {
        e.preventDefault();
        if (e.code == 'Slash') {
            let response = {};
            if (curtar) {
                if (!curtar.detected) {
                    curtar.detected = true;
                    response = {
                        prev: play.prev,
                        ssize: play.ssize,
                        type: 'hit',
                        onset: curtar.onset,
                        ts: e.timeStamp,
                        rt: e.timeStamp - curtar.onset,
                        correct: true,
                    };
                } else {
                    response = {
                        prev: play.prev,
                        ssize: play.ssize,
                        type: 'redundant',
                        onset: curtar.onset,
                        ts: e.timeStamp,
                        rt: e.timeStamp - curtar.onset,
                        correct: false,
                    };
                }
            } else {
                response = {
                    prev: play.prev,
                    ssize: play.ssize,
                    type: 'fa',
                    onset: null,
                    ts: e.timeStamp,
                    rt: null,
                    correct: false,
                };
            }
            play.responses.push(response);
        }
    };

    //start game loop
    play.starttime = await wait_frame_out();
    //register listerner
    let listenman = new ListenerManager();
    listenman.add(document.body, 'keydown', onresponse);
    //game loop
    for (let last_frame = play.starttime, now = await wait_frame_out(); now < play.starttime + play.duration + 2 * play.duration_padding; last_frame = now, now = await wait_frame_out()) {
        context.clearRect(0, 0, cw, ch);
        let frame_interval = now - last_frame;

        //draw canvas bg
        context.save();
        context.fillStyle = gplay.bgcolor;
        context.fillRect(search.canvas.x, search.canvas.y - search.itemr, search.canvas.w, search.canvas.h + 2 * search.itemr);
        context.restore();

        //draw items
        for (let item of items) {
            draw_item(context, item);
        }

        //draw canvas border
        context.save();
        context.fillStyle = gplay.page_bgcolor;
        context.fillRect(search.canvas.x - search.itemr, search.canvas.y - search.itemr, search.itemr, search.canvas.h + search.itemr * 2);
        context.fillRect(search.canvas.x + search.canvas.w, search.canvas.y - search.itemr, search.itemr, search.canvas.h + search.itemr * 2);
        context.restore();

        //update items
        for (let item of items) {
            item.pos[0] -= frame_interval * item.speed;
            if (item.pos[0] < search.canvas.x) {
                let new_x = item.pos[0] + search.canvas.w;
                Object.assign(item, gen_item(search));
                item.pos[0] = new_x;

                //add target if needed
                if (now + frame_interval > play.starttime + targets_onset[tari]) {
                    item.color = get_color(0);
                    curtar = {
                        onset: now,
                        detected: false,
                    };
                    tari++;
                }

            }
        }
        //invalidate target if it is timeout
        if (curtar && now > curtar.onset + gplay.speed.timeout) {
            if (!curtar.detected) {
                play.responses.push({
                    prev: play.prev,
                    ssize: play.ssize,
                    type: 'miss',
                    onset: curtar.onset,
                    ts: now,
                    rt: null,
                    correct: false,
                });
            }
            curtar = null;
        }

    }

    //remove listener
    listenman.removeall();

    //remove canvas
    canvas.remove();

}

/** @returns {Promise<void>} */
async function do_expt(expt, bi, blc) {
    { //instruction
        let html = /*html*/`
        <h2>Experiment Part ${bi + 1} of ${expt.nb}</h2>
        <h3>Instruction</h3>
        <p->Please monitor for a <b>green color dot</b>. Press the <key->/</key-> key once when you see it.</p->
        <p->
        <table>
        <tr>
            <td>Press <key->/</key-> key</td>
            <td><span id="item_00"></span></td>
        </tr>
        <tr>
            <td>Do not press any key</td>
            <td>
                <span id="item_02"></span>
                <span id="item_03"></span>
                <span id="item_04"></span>
                <span id="item_05"></span>
                <span id="item_06"></span>
                <span id="item_07"></span>
                <span id="item_08"></span>
                <span id="item_09"></span>
                <span id="item_10"></span>
            </td>
        </tr>
        </table>
        </p->
        <p->Please respond <b>as accurately and as quickly as possible</b>. </p->
        <p-></p->
        <p->In this part, the green color dot will occur <b>relatively ${blc.prev ? 'frequently' : 'rarely'}</b>. </p->
        `;
        html += "<br>";
        let wait = { type: 'button', text: 'START' };
        await wait_instruction_generic(d1, { html: html, cb: itemcb }, wait);
    }
    { //run block
        //play settings
        let play = {
            duration_padding: 1000,
            duration: 7.5 * 60 * 1000,
            ssize: 30,
            numtar: blc.prev ? 75 : 10,
            prev: blc.prev,
            responses: [],
        };
        await run_play(play);
        expt.responses = expt.responses.concat(play.responses);
    }
} //end expt

/**  
 * @returns {Promise<void>} */
async function do_practice(expt) {
    for (let ntries = 0, allcorrect = false; !allcorrect; ntries++) {
        { //instruction
            let html = "";
            if (ntries == 0) { }
            else html += "<h2>Please try again. Try to be more accurate. </h2>";
            html += /*html*/`
                <h2>Practice</h2>
                <h3>Instruction</h3>
                <p->As you can see, in this experiment, you will be shown with moving color dots on the screen. </p->
                <p->Your task is to monitor for a <b>green</b> color dot, and <u>ignore</u> other dot colors.
                </p->
                <p->When you see a <b>green</b> color dot, press the <key->/</key-> key once immediately.</p->
                <p->Otherwise, you <u>do not</u> need to press a key.</p->
                <p->
                <table>
                <tr>
                    <td>Press <key->/</key-> key</td>
                    <td><span id="item_00"></span></td>
                </tr>
                <tr>
                    <td>Do not press any key</td>
                    <td>
                        <span id="item_02"></span>
                        <span id="item_03"></span>
                        <span id="item_04"></span>
                        <span id="item_05"></span>
                        <span id="item_06"></span>
                        <span id="item_07"></span>
                        <span id="item_08"></span>
                        <span id="item_09"></span>
                        <span id="item_10"></span>
                    </td>
                </tr>
                </table>
                </p->
                <p->In this practice, you will see a green dot for <b>3</b> times.</p->
                `;
            html += "<br>";
            let wait = { type: 'button', text: 'START' };
            await wait_instruction_generic(d1, { html: html, cb: itemcb }, wait);
        }
        { //practice block
            //play settings
            let play = {
                duration_padding: 1000,
                duration: 30 * 1000,
                ssize: 30,
                numtar: 3,
                prev: -1,
                responses: [],
            };
            await run_play(play);
            allcorrect = play.responses.filter(response => response.correct).length >= play.numtar;
        }
    }

    { //good job
        let html = /*html*/`<h2>Good Job</h2>
            <p->Now, the main experiment is about to begin. </p->
            <p->This experiment contains ${expt.nb} parts, which will take about ${7.5 * expt.nb} minutes in total.</p ->
            <p->Until you finish all parts of the experiments, please do not leave the fullscreen mode.</p->
            <br>
        `;
        let wait = { type: 'button', text: 'Next' };
        await wait_instruction_html(d1, html, wait);
    }

}

/**
 * @returns {Promise < void>} */
async function do_example(expt) {
    {
        let html = /*html*/`
        <h2>Practice</h2>
        <h3>Instruction</h3>
        <p->In this experiment, there will be many color dots moving across the screen.</p->
        <br>
        `;
        let wait = { type: 'button', text: 'Take a look' };
        await wait_instruction_generic(d1, { html: html, cb: itemcb }, wait);
    }
    {   //show moving dot sample
        //play settings
        let play = {
            duration_padding: 0,
            duration: 3 * 1000,
            ssize: 30,
            numtar: 0,
            prev: -1,
            responses: [],
        };
        await run_play(play);
    }
}

//consent form
async function consent_form(expt) {
    await wait_instruction_html(d1, consenthtml, { type: 'button', text: 'AGREE' });
}

//show welcome screen
async function do_welcome(expt) {
    let d2 = add({ ele: d1, tag: 'div', class: 'instructiondiv' });
    addhtml(d2, `
            <h1>APPY4005 Cognitive Psychology</h1>
            <h2>Group project experiment</h2>
            <h3>Dynamic visual search and target rate effect</h3>
            <br>
                Press <key->space</key-> to start
                <br>
                    <br>
                        Your display will enter the fullscreen mode in the next page. Please <b>do not leave</b> this mode until the experiment completes.
                        `);

    //press space
    let space_event;
    while ((space_event = await wait_event({ type: 'keydown' })).code != 'Space');
    //save timing diagnostics
    expt.timing_diag = {
        keydown_ts: space_event.timeStamp,
        perfnow_ts: performance.now(),
        frameout_ts: await wait_frame_out()
    };
    d2.remove();

    //go full screen
    d1.classList.add('full');

    if (!await openFullscreen()) {
        addhtml(b1, "<br><br><span style='color:darkred;'>Sorry. Can't enter fullscreen on your computer.</span>");
        await wait_forever();
    }

    //edit to update by using timing_diag
    // if (Math.abs(event2perf) > 5) await wait_instruction_html(d1, "<h2>Unsupported browser</h2>Sorry, your browser is not supported. Please try another browser or update your current browser.", {type: 'forever' });

}

//entry point
async function start_expt() {
    d1 = add({ tag: 'div' });

    //check if parameter sufficient
    let btw = parse_url_params();
    if (!('id' in btw)) await wait_instruction_html(d1, "<h2>Sorry</h2>Access denied.", { type: 'forever' });

    let expt = {
        exptcode: 'dynamic_search',
        datetimestr: get_datetime_str(),
        id: btw.id,
        btw: btw,
        responses: [],
        //for debug only
        browser: navigator.userAgent
    };

    btw.prev_order = rand_int_between(0, 2); //0: rare first, 1: freq first

    //generate block conditions
    let prev_arr = [0, 0, 1, 1]; //0: rare, 1:freq
    if (btw.prev_order) prev_arr.reverse();

    expt.nb = prev_arr.length;

    await do_welcome(expt);
    await consent_form(expt);
    await do_example(expt);
    await do_practice(expt);

    //main expt
    for (let bi = 0; bi < expt.nb; bi++) {
        let blc = { prev: prev_arr[bi % prev_arr.length] };
        await do_expt(expt, bi, blc);
    }

    //leave fullscreen   
    expt.btw.leavefullscreenok = await closeFullscreen();

    //upload
    await do_upload(expt);

    d1.remove();
}