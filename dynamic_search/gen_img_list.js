const fs = require('fs');
const path = require('path');

const img_path = 'images';

let img_dir = fs.readdirSync(img_path);
let images_info = [];
for (let face_fn of img_dir) {
    let fn_match = face_fn.match(/^(F\d{3})\.png$/);
    if (!fn_match) continue;
    let face_id = fn_match[1];
    let mask_fn = `${face_id}_m.png`;
    if (!fs.existsSync(path.join(img_path, mask_fn))) continue;
    images_info.push({
        id: face_id,
        face_fn: path.join(img_path, face_fn),
        mask_fn: path.join(img_path, mask_fn),
    });
}
let images_info_json = JSON.stringify(images_info);
fs.writeFileSync(path.join(img_path, 'img_list.json'), images_info_json);
// console.log(images_info_json);
