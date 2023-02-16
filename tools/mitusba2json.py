import xml.etree.cElementTree as ET
import re
import json
from sys import argv
import os


def str_to_matrix_lis(u):
    m = re.split(' ', u)
    return list(map(float, m))


def str_to_matrix_lis_look(u):
    m = re.split(' ', u)
    lis = list(map(float, m))
    for i in range(len(lis)):
        if i % 2 == 0:
            lis[i] = -lis[i]
    return lis


def str_to_vec(u):
    m = re.split(',', u)
    return list(map(float, m))


def process_shape_type(elem, info):
    shape_type = elem.attrib["type"]
    if shape_type == 'rectangle':
        info["filename"] = 'models/rectangle.obj'
        info['type'] = 'obj'
    elif shape_type == 'cube':
        info["filename"] = 'models/cube.obj'
        info['type'] = 'obj'
    elif shape_type == 'disk':
        info["filename"] = 'models/disk.obj'
        info['type'] = 'obj'
    elif shape_type == 'sphere':
        info['type'] = 'sphere'
        radius_info = elem.find('float')
        info['radius'] = float(radius_info.attrib['value'])
        center_info = elem.find('point')
        x = float(center_info.attrib['x'])
        y = float(center_info.attrib['y'])
        z = float(center_info.attrib['z'])
        info['center'] = [x, y, z]
    elif shape_type == "obj":
        info['type'] = 'obj'


def process_shapes_and_emitters(tree, res):
    res['shapes'] = []
    res['emitters'] = []
    for elem in tree.findall("shape"):
        temp = {}
        process_shape_type(elem, temp)
        for ele in elem.findall("string"):
            temp["filename"] = ele.attrib["value"]
        for ele in elem.findall("transform"):
            temp["transform"] = {}
            for el in ele:
                temp["transform"]["type"] = el.tag
                temp["transform"]["value"] = str_to_matrix_lis(el.attrib["value"])
        if elem.find('emitter') is not None:
            emitter = elem.find('emitter')
            emit = {'type': 'area'}
            rgb = emitter.find('rgb')
            emit['radiance'] = str_to_vec(rgb.attrib['value'])
            emit['shape'] = temp
            res['emitters'].append(emit)
        else:
            temp['bsdf'] = {}
            temp['bsdf']['ref'] = elem.find('ref').attrib['id']
            res['shapes'].append(temp)


bsdf_dict = {
    "roughconductor": 'roughconductor',
    'diffuse': 'diffuse',
    'dielectric': 'dielectric',
    'roughdielectric': 'dielectric',
    'thindielectric': 'dielectric',
    'conductor': 'conductor',
    'plastic': 'plastic',
    'roughplastic': 'plastic'
}


def process_textures(bsdf_info, bsdf):
    bsdf['textures'] = []
    for rgb in bsdf_info.findall('rgb'):
        texture = {}
        texture['name'] = rgb.attrib['name']
        texture['type'] = 'single_color'
        texture['value'] = str_to_vec(rgb.attrib['value'])
        bsdf['textures'].append(texture)
    for tex in bsdf_info.findall('texture'):
        if tex.attrib['type'] == 'bitmap':
            texture = {}
            texture['type'] = 'file'
            texture['name'] = tex.attrib['name']
            for info in tex.findall('string'):
                texture[info.attrib['name']] = info.attrib['value']
            bsdf['textures'].append(texture)


def process_bsdf(bsdf_info):
    bsdf = {}
    if 'id' in bsdf_info.attrib:
        bsdf['id'] = bsdf_info.attrib['id']

    bsdf_mitsuba_type = bsdf_info.attrib['type']

    if bsdf_mitsuba_type in bsdf_dict:
        bsdf['type'] = bsdf_dict[bsdf_mitsuba_type]
        for ele in bsdf_info.findall('float'):
            bsdf[ele.attrib['name']] = float(ele.attrib['value'])
        process_textures(bsdf_info, bsdf)

    # if bsdf_info.attrib['type'] == 'roughconductor':
    #     bsdf['type'] = 'roughconductor'
    #     for ele in bsdf_info.findall('float'):
    #         bsdf[ele.attrib['name']] = float(ele.attrib['value'])
    # elif bsdf_info.attrib['type'] == "diffuse":
    #     bsdf['type'] = 'diffuse'
    # elif bsdf_info.attrib['type'] == 'dielectric' or bsdf_info.attrib['type'] == 'roughdielectric' or bsdf_info.attrib[
    #     'type'] == 'thindielectric':
    #     bsdf['type'] = 'dielectric'
    # elif bsdf_info.attrib['type'] == 'conductor':
    #     bsdf['type'] = 'conductor'
    # elif bsdf_info.attrib['type'] == 'plastic' or bsdf_info.attrib['type'] == 'roughplastic':
    #     bsdf['type'] = 'plastic'

    else:
        bsdf['type'] = 'diffuse'
        bsdf['textures'] = []
        tex = {}
        tex['name'] = 'reflectance'
        tex['type'] = 'single_color'
        tex['value'] = [0.2, 0.2, 0.2]
        bsdf['textures'].append(tex)
    return bsdf


def process_bsdfs(tree, res):
    res['bsdfs'] = []
    for elem in tree.findall('bsdf'):
        bsdf = {}
        if elem.find('bsdf') is not None:
            bsdf_info = elem.find('bsdf')
            if bsdf_info.find('bsdf') is not None:
                b_info = bsdf_info.find('bsdf')
                print(b_info.attrib, b_info.tag)
                if 'id' in b_info.attrib:
                    bsdf['id'] = b_info.attrib['id']
                bsdf = process_bsdf(b_info)
                if 'id' in bsdf_info.attrib:
                    bsdf['id'] = bsdf_info.attrib['id']
            else:
                bsdf = process_bsdf(bsdf_info)
        else:
            bsdf = process_bsdf(elem)
        if 'id' in elem.attrib:
            bsdf['id'] = elem.attrib['id']
        res['bsdfs'].append(bsdf)


def process_camera(tree, res):
    res['camera'] = {}
    res['camera']['type'] = 'pinhole'
    came_info = tree.find('sensor')
    for info in came_info.findall('float'):
        res['camera']['fov'] = float(info.attrib['value'])
    trans = came_info.find('transform')
    for ele in trans:
        print(ele.tag)
    if trans.find('matrix') is not None:
        trans_mat = trans.find('matrix')
        res['camera']['matrix'] = str_to_matrix_lis_look(trans_mat.attrib['value'])
    elif trans.find('lookat') is not None:
        look_info = trans.find('lookat')
        res['camera']['target'] = str_to_vec(look_info.attrib['target'])
        res['camera']['up'] = str_to_vec(look_info.attrib['up'])
        res['camera']['orig'] = str_to_vec(look_info.attrib['origin'])
    film = came_info.find('film')
    for info in film.findall('integer'):
        res['camera'][info.attrib['name']] = int(info.attrib['value'])
    v = float(res['camera']['height'])/float(res['camera']['width'])
    res['camera']['fov']*=v


def basic_setting(res):
    res['sampler'] = {}
    res['sampler']['type'] = 'pcg_random'
    res['sample_count'] = 64
    res['integrator'] = {}
    res['integrator']['type'] = 'mis'


def mitsuba_to_phoenix(file_name):
    tree = ET.ElementTree(file=file_name)
    res = {}
    basic_setting(res)
    process_camera(tree, res)
    process_bsdfs(tree, res)
    process_shapes_and_emitters(tree, res)
    return res


def write_file(data, filename):
    with open(filename, 'w') as f:
        json.dump(data, f, indent=4, separators=(',', ':'))


if __name__ == '__main__':
    file_path = argv[1]
    if not os.path.exists(file_path):
        raise "no such file!"
    file_path = os.path.abspath(file_path)
    data = mitsuba_to_phoenix(file_path)
    path, filename = os.path.split(file_path)
    name, ext = os.path.splitext(filename)
    path = os.path.abspath(path)
    write_file(data, os.path.join(path, name + '.json'))
