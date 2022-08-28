var lightness = 70;
var sat = 70;
var tarhue = 145;
var ncolor = 12;
var gplay = {
    speed: {
        mean: 2000,
        range: 500,
        timeout: 2500,
    },
    page_bgcolor: 'white',
    bgcolor: chroma.lch(90, 0, 0),
};

//set item
function get_color(n) {
    return chroma.lch(lightness, sat, (tarhue + n * 360 / ncolor) % 360);
}

//gen item
function gen_item(search) {
    return {
        r: search.itemr,
        pos: [search.canvas.x + Math.random() * search.canvas.w, search.canvas.y + Math.random() * search.canvas.h],
        color: get_color(rand_int_between(2, ncolor - 1)),
        speed: search.canvas.w / (gplay.speed.mean + (Math.random() - .5) * gplay.speed.range),
    };
}

/**draw item
 * @param {CanvasRenderingContext2D} context 
*/
function draw_item(context, item) {
    context.save();
    context.fillStyle = item.color;
    context.fill(path_circle(...item.pos, item.r));
    context.restore();
}

/**
 *draw item callback function 
 * @param {HTMLElement} d2 
 */
let itemcb = function (d2) {
    let allidchild = d2.querySelectorAll('[id]');
    for (let child of allidchild) {
        let match = child.id.match(/^item_(\d{2})$/);
        if (match) {
            let item = {
                color: get_color(parseInt(match[1])),
                r: 15,
            };
            add_simple_canvas(child, 32, 32, function (cx, cy, cw, ch, context) {
                draw_item(context, {
                    ...item,
                    pos: [cx, cy]
                });
            });
        }
    }
};

var consenthtml =/*html*/`
<div style="font-size: smaller;">
<h2>Hong Kong Baptist University</h2>
<h1>CONSENT TO PARTICIPATE IN RESEARCH</h1>
<h3>Human performance in dynamic visual search</h3>
<p>You are invited to participate in a research study conducted by Louis Chan from the Psychology Unit of
    the
    Hong Kong Baptist University. This experiment is a portion of the APPY4005 Cognitive Psychology class project. The aim of this research is to study the characteristics of human
    performance
    in real-life visual search. This experiment will be a computer-based task, in which you monitor for a
    particular color a number of
    moving circle dots. You will make your responses by using a computer keyboard. The whole experiment will
    last for about 30 minutes.</p>
<p>This experiment should not cause any psychological or physical hazard to you. In order to minimize
    fatigue or
    discomforts, you are advised to take short breaks between the experimental blocks.</p>
<p>Your participation is voluntary. If you decide to participate, you are free to withdraw at any time. </p>
<p>Any personal information obtained in this study will remain confidential. Data recorded in the experiment
    will be used for research purposes only. </p>
<h2>QUESTIONS AND CONCERNS</h2>
<p>If you have any questions or concerns about this research, please feel free to contact Louis Chan by
    email
    (clouis@hkbu.edu.hk) or by phone (3411-3063). If you have questions about your rights as a research
    participant, please contact Research Ethics
    Committee by email (hkbu_rec@hkbu.edu.hk) or by mail to Graduate School, Hong Kong Baptist University,
    Kowloon Tong, Hong Kong.</p>
<h2>DECLARATION</h2>
<p>By clicking the AGREE button, I declare that I understand the procedures described above and agree to
    participate in this study.</p>
</div>
`;